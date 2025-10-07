#!/bin/bash

BIGEND="$1"
echo "$(echo "$BIGEND" | tac -rs .. | echo "$(tr -d '\n')")"
