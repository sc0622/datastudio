#-------------------------------------------------
#
# Project created by QtCreator 2016-08-31T18:09:05
#
#-------------------------------------------------

ROOT = $$section(PWD, '/', -1)

#-------------------------------------------------

$$compiler($$group($${ROOT}.header), $$group(echo --- $$PWD))

#-------------------------------------------------

$$compiler($$group($${ROOT}.bin), $$removedir($$this_dir/setup/pack/bin/))
