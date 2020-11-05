import os
import pandas as pd
import matplotlib as mpl
if os.environ.get('DISPLAY','') == '':
        print('no display found. Using non-interactive Agg backend')
        mpl.use('Agg')
import matplotlib.pyplot as plt


#tmp = pd.read_csv('./433.log', sep='\s+', header=None)
#skip_list = tmp.index[tmp.iloc[:,0].str.contains('top')].to_list()

#log = pd.read_csv('./433.log', header=None, sep='\s+', skiprows=skip_list)
log = pd.read_csv('./433.log', header=None, sep='\s+')

log.columns = ['pid' ,'usr', 'pr','ni', 'virt', 'res','shr', 's', 'cpu', 'mem','duration', 'command']
#log = log[log['command']=='bzip2_base.gcc4']
log = log[log['command']=='milc_base.gcc43']

log['physic'] = log.apply(lambda row:(row['res'] + row['shr'])/1024, axis = 1) 
log['virt'] = log['virt'].apply(lambda x : x/1024)
print log.virt.max(), log.physic.max()


#fig, (axc, axm) = plt.subplots(nrows=2, ncols=1, sharex=True)
fig, axm = plt.subplots()

#axc.plot(log.duration, log.cpu, label="CPU")
axm.plot(log.duration, log.virt, ms=2,linestyle='', marker='.',  label="Virtual")
axm.plot(log.duration, log.physic, ms=2, linestyle='', marker ='.', label="Physical")

#axc.set_ylabel("CPU Usage")
axm.set_ylabel("MEM Usage (MiB)")
axm.set_xlabel("Duration")

axm.set_title("Memory Usaage of 433.milc")
plt.savefig("./433_memory.png", format="png")

