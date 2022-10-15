from building import *
import rtconfig

cwd     = GetCurrentDir()

src    = Glob('drv/*.h')
src    += Glob('drv/*.c')
src    += Glob('src/*.h')
src    += Glob('src/*.c')

path    = [cwd + "/"]
path   += [cwd + '/src']
path   += [cwd + '/drv']

LOCAL_CCFLAGS = ''

group = DefineGroup('nv7lidar', src, depend = ['PKG_USING_LRF_NV7LIDAR'], CPPPATH = path, LOCAL_CCFLAGS = LOCAL_CCFLAGS)

Return('group')