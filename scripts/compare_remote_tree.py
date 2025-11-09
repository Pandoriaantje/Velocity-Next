import json
import os
from urllib.request import urlopen

REPO_TREE_URL = "https://api.github.com/repos/Gualdimar/Velocity/git/trees/master?recursive=1"


def fetch_remote_paths():
    with urlopen(REPO_TREE_URL) as response:
        data = json.load(response)
    return {
        entry["path"].replace("/", os.sep)
        for entry in data["tree"]
        if entry.get("type") == "blob"
    }


def collect_local_paths(root):
    local_paths: set[str] = set()
    for current_root, _dirs, files in os.walk(root):
        for name in files:
            rel_path = os.path.relpath(os.path.join(current_root, name), root)
            local_paths.add(rel_path)
    return local_paths


def main():
    workspace_root = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))
    remote_paths = fetch_remote_paths()
    local_paths = collect_local_paths(workspace_root)

    missing = sorted(remote_paths - local_paths)
    extra = sorted(local_paths - remote_paths)

    print(f"Missing (remote but not local): {len(missing)}")
    for path in missing[:200]:
        print(f"M: {path}")
    if len(missing) > 200:
        print("...")

    print(f"Extra (local but not remote): {len(extra)}")
    for path in extra[:200]:
        print(f"E: {path}")
    if len(extra) > 200:
        print("...")


if __name__ == "__main__":
    main()
