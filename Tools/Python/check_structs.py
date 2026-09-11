import unreal

targets = [
    "/Script/TenTenTown.CharacterBaseDataTable",
    "/Script/TenTenTown.CharacterStaminaDataTable",
]

for t in targets:
    obj = unreal.load_object(None, t)
    unreal.log(f"{t} -> {obj}")
