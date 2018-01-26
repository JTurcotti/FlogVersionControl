Flog is an implementation of git version control software. Its current features, specified below, are a proper subset of those implemented in the original git.

For implementation details, see DESIGN

Usage:
flog <command> <args>
(alll commands after flog init must be run in same directory unless portability is enabled)

Commands:
Init - create empty flog repo in current dir

Add <file> - add file to index

Commit [-m <"message">] - builds commit from index and adds to current branch

Log - Display list of all commits to root from current branch

Branch [-d <branch-name>] - switch to given branch or create if nonexistant, option -d deletes branch

Checkout <commit/branch name> | (-b <branch-name) - move to position given, with otion -b create branch first, update working directory to match

Reset [--hard | --soft] <commit-name/other position> or default: HEAD - three stages: move HEAD; update index; update working tree, by default peforms first two; --soft performs only first; --hard performs all three

For furthur questions:
Find me and ask me