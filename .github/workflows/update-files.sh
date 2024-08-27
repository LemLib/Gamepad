#!/bin/bash

# git remote add lemlib https://github.com/LemLib/LemLib.git
git fetch lemlib
while IFS= read -r file_name; read -r commit_msg; do
    git checkout lemlib/stable -- $file_name
    if IFS= git status -s | grep "^[MTADRCU]" | read -d '' -n 1; then
        git commit --message="$commit_msg"
    fi
done < .github/workflows/update-files-list.txt