import pathlib
import yaml

# Папки, где ищем YAML
folders = ["scoreboard", ".github"]

# Находим все YAML-файлы
yaml_files = []
for folder in folders:
    yaml_files.extend(pathlib.Path(folder).rglob("*.yml"))
    yaml_files.extend(pathlib.Path(folder).rglob("*.yaml"))

for file_path in yaml_files:
    print(f"Проверка {file_path}")

    try:
        # Читаем и проверяем синтаксис
        with open(file_path, "r", encoding="utf-8") as f:
            content = f.read()
            yaml.safe_load(content)
    except yaml.YAMLError as e:
        print(f"{file_path}\n  Ошибка синтаксиса: {e}")

    # Проверяем наличие пустой строки в конце файла
    if not content.endswith("\n"):
        print(f"{file_path}\n  Добавляем пустую строку в конце файла")
        with open(file_path, "a", encoding="utf-8") as f:
            f.write("\n")
