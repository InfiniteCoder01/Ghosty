@echo off
cls
em++ src/main.cpp -o html/ghosty.html --bind -sASYNCIFY --shell-file basic_template.html --preload-file Assets -s ALLOW_MEMORY_GROWTH=1 --use-preload-plugins
