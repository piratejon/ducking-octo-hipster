set -o vi

export CFLAGS="-g3 -Wall -std=c99 -ansi -pedantic"

export LESS=-FiXS

export PATH=$PATH:$HOME/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/lib

export PS1='\[\e[1;33m\][\u@\h \W]\$\[\e[0m\] '

alias tl='tmux ls'
alias ta='tmux attach -t'
alias tn='tmux new -s'
alias vg='valgrind --tool=memcheck --num-callers=40 --leak-check=full --leak-resolution=high --show-reachable=yes --track-origins=yes -v --log-file=vg.%p'

