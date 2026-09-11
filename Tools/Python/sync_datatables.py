# -*- coding: utf-8 -*-
import os
import unreal

# ============================================================
# CONFIG
# ============================================================

CONTENT_FOLDERS = ["/Game"]  # DT가 많아도 /Game 하나로 OK

# 콘텐츠 경로 prefix -> 프로젝트 루트 기준 RawData 폴더
# 예: /Game/Character/Fighter/... 에 있는 DT들은 RawData/Character/Fighter/... 에서 CSV를 찾음
RULES = [
    ("/Game/Characters/Fighter/DataTable", "RawData/Character/Fighter"),
    # ("/Game/Character/Mage",    "RawData/Character/Mage"),
    # ("/Game/Monster",          "RawData/Monster"),
    # ("/Game/Structure",        "RawData/Structure"),
]

RECURSIVE_SCAN = True
DRY_RUN = False
SAVE_ASSETS = True

# 콘텐츠 폴더 구조에 이런 중간 폴더가 끼어있으면 CSV 경로 계산 시 제거
NOISE_DIRS = {"datatable", "datatables", "dt"}

# ============================================================
# Utils
# ============================================================

def _project_abs(*parts) -> str:
    proj_dir = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
    return os.path.normpath(os.path.join(proj_dir, *parts))

def _norm_csv_root(csv_root: str) -> str:
    if os.path.isabs(csv_root):
        return os.path.normpath(csv_root)
    return _project_abs(csv_root)

def _assetdata_to_object_path(a: unreal.AssetData) -> str:
    # UE 버전에 따라 object_path 접근이 깨지는 경우가 있어 package_name+asset_name으로 안전 생성
    pkg = str(a.package_name)  # "/Game/A/B/DT_X"
    name = str(a.asset_name)   # "DT_X"
    return f"{pkg}.{name}"     # "/Game/A/B/DT_X.DT_X"

def _split_object_path(object_path: str):
    # "/Game/A/B/DT_X.DT_X" -> ("/Game/A/B", "DT_X")
    pkg = object_path.split(".")[0]
    package_path = pkg.rsplit("/", 1)[0]
    asset_name = pkg.rsplit("/", 1)[1]
    return package_path, asset_name

def _is_datatable(a: unreal.AssetData) -> bool:
    try:
        if hasattr(a, "asset_class_path"):
            return a.asset_class_path.asset_name == "DataTable"
        return a.asset_class == "DataTable"
    except Exception:
        return False

def _best_rule_for(package_path: str):
    # 가장 긴 prefix가 우선(세부 폴더 규칙이 이기도록)
    best = None
    for content_prefix, csv_root in RULES:
        if package_path.startswith(content_prefix):
            if best is None or len(content_prefix) > len(best[0]):
                best = (content_prefix, csv_root)
    return best

def _strip_noise_dirs(rel_path_from_prefix: str) -> str:
    parts = [p for p in rel_path_from_prefix.replace("\\", "/").split("/") if p]
    cleaned = [p for p in parts if p.lower() not in NOISE_DIRS]
    return "/".join(cleaned)

def _get_saved_csv_from_dt(dt: unreal.DataTable):
    """
    DataTable이 기억하고 있는 원본 CSV 경로(절대경로)를 가져옴.
    없거나 파일이 실제로 없으면 None.
    """
    try:
        import_data = dt.get_editor_property("asset_import_data")
        if not import_data:
            return None
        # AssetImportData.get_first_filename()는 첫 소스 파일 경로를 반환 :contentReference[oaicite:1]{index=1}
        f = import_data.get_first_filename()
        if f and os.path.exists(f):
            return os.path.normpath(f)
        return None
    except Exception:
        return None

def _log_header(msg: str):
    unreal.log("==============================")
    unreal.log(msg)
    unreal.log("==============================")

# ============================================================
# Main
# ============================================================

def sync_datatables():
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    total = 0
    ok = 0
    skipped = 0
    fails = []

    _log_header("TenTenTown DataTable Sync START")

    seen = set()
    for folder in CONTENT_FOLDERS:
        assets = asset_registry.get_assets_by_path(folder, recursive=RECURSIVE_SCAN)

        for a in assets:
            if not _is_datatable(a):
                continue

            obj_path = _assetdata_to_object_path(a)
            if obj_path in seen:
                continue
            seen.add(obj_path)

            total += 1
            package_path, asset_name = _split_object_path(obj_path)

            dt = unreal.load_object(None, obj_path)
            if not dt:
                skipped += 1
                unreal.log_warning(f"[SKIP] Load failed: {obj_path}")
                continue

            if not getattr(dt, "row_struct", None):
                skipped += 1
                unreal.log_warning(f"[SKIP] Missing RowStruct: {obj_path}")
                continue

            # 1) DT가 기억한 원본 CSV가 있으면 그걸 우선 사용 (리임포트 효과)
            csv_file = _get_saved_csv_from_dt(dt)

            # 2) 없으면 RULES로 CSV 경로 계산
            if not csv_file:
                matched = _best_rule_for(package_path)
                if not matched:
                    skipped += 1
                    unreal.log_warning(f"[SKIP] No RULE for: {package_path} ({obj_path})")
                    continue

                content_prefix, csv_root = matched
                csv_root_abs = _norm_csv_root(csv_root)

                rel = package_path[len(content_prefix):]
                rel_clean = _strip_noise_dirs(rel)

                csv_dir = os.path.normpath(os.path.join(csv_root_abs, rel_clean))
                csv_file = os.path.normpath(os.path.join(csv_dir, f"{asset_name}.csv"))

                if not os.path.exists(csv_file):
                    skipped += 1
                    unreal.log_warning(f"[SKIP] Missing CSV: {csv_file}  <= {obj_path}")
                    continue

            # 3) ImportTask로 덮어쓰기(= 업데이트)
            factory = unreal.DataTableFactory()
            factory.struct = dt.row_struct

            task = unreal.AssetImportTask()
            task.filename = csv_file
            task.destination_path = package_path
            task.destination_name = asset_name
            task.replace_existing = True
            task.automated = True
            task.save = SAVE_ASSETS
            task.factory = factory

            if DRY_RUN:
                unreal.log(f"[DRY] Update: {obj_path} <= {csv_file}")
                ok += 1
                continue

            try:
                asset_tools.import_asset_tasks([task])
                unreal.log(f"[OK] Update: {obj_path} <= {csv_file}")
                ok += 1
            except Exception as e:
                fails.append((obj_path, str(e)))
                unreal.log_error(f"[FAIL] {obj_path} ({e})")

    _log_header("TenTenTown DataTable Sync DONE")
    unreal.log(f"Total={total} | OK={ok} | Skipped={skipped} | Failed={len(fails)}")

    if fails:
        unreal.log_warning("---- FAIL LIST ----")
        for p, reason in fails:
            unreal.log_warning(f"{p} -> {reason}")

sync_datatables()
