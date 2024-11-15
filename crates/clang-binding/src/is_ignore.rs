use ignore::WalkBuilder;
use std::path::PathBuf;

pub fn is_ignored(path: &PathBuf) -> bool {
    let walker = WalkBuilder::new(path.parent().unwrap())
        .standard_filters(false)
        .hidden(true)
        .parents(true)
        .max_depth(Some(1))
        .add_custom_ignore_filename(".clang-format-ignore")
        .build();

    for entry in walker {
        if let Ok(entry) = entry {
            if entry.path() == path {
                return false;
            }
        }
    }

    true
}
