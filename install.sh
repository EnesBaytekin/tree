#!/bin/bash
if [[ -f /usr/bin/tree ]] then
else
    sudo wget https://github.com/EnesBaytekin/tree/releases/download/v1.0.1/tree -P /usr/bin
    sudo chmod +x /usr/bin/tree
    sudo cp _tree_completions /usr/share/bash-completion/completions/tree
fi
