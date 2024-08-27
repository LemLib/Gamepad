#!/bin/bash

# git remote add lemlib https://github.com/LemLib/LemLib.git
git fetch lemlib
while IFS= read -r file_name commit_msg; do
    git checkout lemlib/stable -- $file_name
    git -a -m "$commit_msg"
done < update-files-list.txt