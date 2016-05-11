#! /usr/bin/python2

from subprocess import *
from multiprocessing import Process
import time

def runShell(shellName, outputName, testarray, testTab=0):
    p = Popen(shellName, stdin=PIPE)
    Popen(["./memmonitor.sh", shellName, outputName])

    (fin, fout) = (p.stdin, p.stdout)
    
    #fin.write("connect\n")
    #fin.flush()

    index = 0

    while True:
        fin.write(testarray[index])
        if (testTab):
            for i in range(10):
                fin.write("\t")

        fin.write("\n")
        fin.flush()

        index+=1
        if index >= len(testarray):
            index = 0
	time.sleep(0.2)
    
    fin.close()


def main():
    completeTest = ["send r", "send c", "action b", "send ", "s"]
    #completeTest = ["action Reload"]
    
    p1 = Process(target=runShell, args=("./vt_shell", "complete", completeTest, 1))
    p1.start()

    rtpTest = ["send VOIP_RTP_ACTIVE -d 3 -D 0 -p 0 -P 20 0"]
    #rtpTest = ["reload"]
    #rtpTest = ["action BeCalled 0 123"]
    p2 = Process(target=runShell, args=("./test_vt_shell", "rtpactive", rtpTest))
    p2.start()

    p1.join()
    p2.join()
        
    print "All tests done.\n"

main()
