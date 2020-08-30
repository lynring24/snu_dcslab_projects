import os
import pandas as pd
import matplotlib as mpl
if os.environ.get('DISPLAY','') == '':
        print('no display found. Using non-interactive Agg backend')
        mpl.use('Agg')
import matplotlib.pyplot as plt


tmp = pd.read_csv('./bzip2.log', sep='\s+', header=None)
skip_list = tmp.index[tmp.iloc[:,0].str.contains('top')].to_list()

log = pd.read_csv('./bzip2.log', header=None, sep='\s+', skiprows=skip_list)

log.columns = ['pid' ,'usr', 'pr','ni', 'virt', 'res','shr', 's', 'cpu', 'mem','duration', 'command']
log = log[log['command']=='bzip2_base.gcc4']


fig, (axc, axm) = plt.subplots(nrows=2, ncols=1, sharex=True)

axc.plot(log.duration, log.cpu, label="CPU")
axm.plot(log.duration, log.mem, label="MEM")

axc.set_ylabel("CPU Usage")
axm.set_ylabel("MEM Usage")
axm.set_xlabel("Duration")

axc.set_title("CPU, Memory Usaage")
plt.savefig("./plot.png", format="png")

