# -*- coding: utf-8 -*-
import os
import json
import unreal

MANIFEST_REL = "RawData/dt_manifest.json"
SAVE_ASSETS = True

def project_dir() -> str:
    return unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())

def abs_path(rel_or_abs: str) -> str:
    if os.path.isabs(rel_or_abs):
        return os.path.normpath(rel_or_abs)
    return os.path.normpath(os.path.join(project_dir(), rel_or_abs))

def log_header(msg: str):
    unreal.log("==============================")
    unreal.log(msg)
    unreal.log("==============================")

def is_datatable_assetdata(a: unreal.AssetData) -> bool:
    try:
        if hasattr(a, "asset_class_path"):
            return a.asset_class_path.asset_name == "DataTable"
        return a.asset_class == "DataTable"
    except Exception:
        return False

def find_dt_asset_path(dt_name: str, search_root: str) -> str | None:
    """returns asset_path like '/Game/.../DT_X'"""
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    assets = ar.get_assets_by_path(search_root, recursive=True)
    for a in assets:
        if not is_datatable_assetdata(a):
            continue
        if str(a.asset_name) == dt_name:
            return str(a.package_name)  # /Game/.../DT_X
    return None

def load_dt_object(asset_path: str, dt_name: str):
    """asset_path: /Game/.../DT_X"""
    obj_path = f"{asset_path}.{dt_name}"
    return unreal.load_object(None, obj_path)

def get_row_struct(dt_obj):
    try:
        return dt_obj.get_editor_property("row_struct")
    except Exception:
        return getattr(dt_obj, "row_struct", None)

def ensure_dt(asset_path: str, dt_name: str, desired_struct):
    """
    - DT가 없으면 생성
    - DT가 있는데 RowStruct가 다르면 삭제 후 재생성
    return: dt_obj, final_asset_path(/Game/.../DT_X)
    """
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    package_path = asset_path.rsplit("/", 1)[0]  # /Game/.../DataTableFolder
    final_asset_path = f"{package_path}/{dt_name}"

    # 존재 여부
    if unreal.EditorAssetLibrary.does_asset_exist(final_asset_path):
        dt = load_dt_object(final_asset_path, dt_name)
        if not dt:
            # 로드 실패면 삭제 후 재생성
            unreal.log_warning(f"[RECREATE] Load failed -> delete & recreate: {final_asset_path}")
            unreal.EditorAssetLibrary.delete_asset(final_asset_path)
            dt = None
        else:
            cur_struct = get_row_struct(dt)
            if (cur_struct is None) or (cur_struct != desired_struct):
                unreal.log_warning(
                    f"[RECREATE] RowStruct mismatch: {final_asset_path} "
                    f"(current={cur_struct.get_path_name() if cur_struct else None}, "
                    f"desired={desired_struct.get_path_name()})"
                )
                unreal.EditorAssetLibrary.delete_asset(final_asset_path)
                dt = None
    else:
        dt = None

    # 생성
    if not dt:
        factory = unreal.DataTableFactory()
        # factory.struct는 set_editor_property로 넣는 게 안전
        factory.set_editor_property("struct", desired_struct)

        dt = asset_tools.create_asset(dt_name, package_path, unreal.DataTable, factory)
        if not dt:
            raise RuntimeError(f"Could not create DataTable asset: {final_asset_path}")

    return dt, final_asset_path

def fill_from_csv(dt_obj, csv_abs: str) -> bool:
    """
    DataTableFunctionLibrary로 CSV를 읽어 채웁니다.
    CSV 형식이 DataTable 요구사항에 안 맞으면 여기서 실패합니다.
    """
    try:
        # 보통 bool 반환(버전에 따라 None일 수도 있음)
        result = unreal.DataTableFunctionLibrary.fill_data_table_from_csv_file(dt_obj, csv_abs)
        return bool(result) if result is not None else True
    except Exception as e:
        unreal.log_error(f"[FILL FAIL] {e}")
        return False

def main():
    manifest_abs = abs_path(MANIFEST_REL)
    if not os.path.exists(manifest_abs):
        unreal.log_error(f"[FAIL] manifest not found: {manifest_abs}")
        return

    with open(manifest_abs, "r", encoding="utf-8") as f:
        manifest = json.load(f)

    entries = manifest.get("entries", [])
    if not entries:
        unreal.log_warning("[WARN] manifest entries is empty")
        return

    log_header("DataTable Sync (manifest) START")
    unreal.log(f"[INFO] project_dir = {project_dir()}")
    unreal.log(f"[INFO] manifest    = {manifest_abs}")

    ok = 0
    fail = 0

    for e in entries:
        dt_name = e["dt_name"]
        search_root = e.get("search_root", "/Game")
        struct_path = e["row_struct"]
        csv_abs = abs_path(e["csv_rel"])

        # CSV 존재
        if not os.path.exists(csv_abs):
            fail += 1
            unreal.log_error(f"[FAIL] CSV missing: {csv_abs} (DT={dt_name})")
            continue

        # RowStruct 로드
        desired_struct = unreal.load_object(None, struct_path)
        if not desired_struct:
            fail += 1
            unreal.log_error(f"[FAIL] RowStruct load failed: {struct_path} (DT={dt_name})")
            continue

        # DT 위치 찾기
        asset_path = find_dt_asset_path(dt_name, search_root)
        if not asset_path:
            fail += 1
            unreal.log_error(f"[FAIL] DT not found: {dt_name} (root={search_root})")
            continue

        try:
            dt_obj, final_asset_path = ensure_dt(asset_path, dt_name, desired_struct)

            # CSV 채우기
            if not fill_from_csv(dt_obj, csv_abs):
                fail += 1
                unreal.log_error(f"[FAIL] FillDataTableFromCSV failed: {final_asset_path} <= {csv_abs}")
                continue

            # 저장
            if SAVE_ASSETS:
                unreal.EditorAssetLibrary.save_asset(final_asset_path)

            ok += 1
            unreal.log(f"[OK] {final_asset_path} <= {csv_abs}")

        except Exception as ex:
            fail += 1
            unreal.log_error(f"[FAIL] {dt_name}: {ex}")

    log_header("DataTable Sync (manifest) DONE")
    unreal.log(f"Total={len(entries)} | OK={ok} | FAIL={fail}")

main()
