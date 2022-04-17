mutex=$1
file=$2
superstep=$3
source_id=$4
host_num=$5
partition_opt=$6
file_name=$7

#RDMA_PageRank
nohup ./RDMA_PageRank/PiGraph_RDMA_PageRank -m $mutex -f $file -n $host_num -s $superstep -p $partition_opt 1 > execution_log/RDMA_PageRank_$file_name.log_$host_num 2>&1
sleep 60s
nohup ./RDMA_PageRank/PiGraph_RDMA_PageRank -m $mutex -f $file -n $host_num -s $superstep -p $partition_opt 1 > check_resource_execution_log/RDMA_PageRank_$file_name.log_$host_num 2>&1 &
pidof PiGraph_RDMA_PageRank > pid
RDMA_PageRank_pid=`cat pid`

nohup pidstat -I -p $RDMA_PageRank_pid 1 > test_log/cpu/RDMA_PageRank_$file_name.log_cpu_$host_num 2>&1 &
nohup pidstat -r -p $RDMA_PageRank_pid 1 > test_log/mem/RDMA_PageRank_$file_name.log_mem_$host_num 2>&1 &

while :
do
    pidof PiGraph_RDMA_PageRank > pid
    check_pid=`cat pid`
    if [ "$check_pid" == "" ]
    then
        echo RDMA PageRank end
        break
    fi
done

sleep 60s

#RDMA_SSSP
nohup ./RDMA_SSSP/PiGraph_RDMA_SSSP -m $mutex -f $file -n $host_num -s $superstep -p $partition_opt -d $source_id 1 > execution_log/RDMA_SSSP_$file_name.log_$host_num 2>&1
sleep 60s
nohup ./RDMA_SSSP/PiGraph_RDMA_SSSP -m $mutex -f $file -n $host_num -s $superstep -p $partition_opt -d $source_id 1 > check_resource_execution_log/RDMA_SSSP_$file_name.log_$host_num 2>&1 &
pidof PiGraph_RDMA_SSSP > pid
RDMA_SSSP_pid=`cat pid`

nohup pidstat -I -p $RDMA_SSSP_pid 1 > test_log/cpu/RDMA_SSSP_$file_name.log_cpu_$host_num 2>&1 &
nohup pidstat -r -p $RDMA_SSSP_pid 1 > test_log/mem/RDMA_SSSP_$file_name.log_mem_$host_num 2>&1 &

while :
do
    pidof PiGraph_RDMA_SSSP > pid
    check_pid=`cat pid`
    if [ "$check_pid" == "" ]
    then
        echo RDMA SSSP end
        break
    fi

done

sleep 60s

#IPoIB_PageRank
nohup ./Ethernet_IPoIB_PageRank/PiGraph_IPoIB_PageRank -m $mutex -f $file -n $host_num -s $superstep -N ipoib -p $partition_opt  1 > execution_log/IPoIB_PageRank_$file_name.log_$host_num 2>&1
sleep 60s
nohup ./Ethernet_IPoIB_PageRank/PiGraph_IPoIB_PageRank -m $mutex -f $file -n $host_num -s $superstep -N ipoib -p $partition_opt  1 > check_resource_execution_log/IPoIB_PageRank_$file_name.log_$host_num 2>&1 &
pidof PiGraph_IPoIB_PageRank > pid
IPoIB_PageRank_pid=`cat pid`

nohup pidstat -I -p $IPoIB_PageRank_pid 1 > test_log/cpu/IPoIB_PageRank_$file_name.log_cpu_$host_num 2>&1 &
nohup pidstat -r -p $IPoIB_PageRank_pid 1 > test_log/mem/IPoIB_PageRank_$file_name.log_mem_$host_num 2>&1 &

while :
do
    pidof PiGraph_IPoIB_PageRank > pid
    check_pid=`cat pid`
    if [ "$check_pid" == "" ]
    then
        echo IPoIB PageRank end
        break
    fi
    
done

sleep 60s

#IPoIB_SSSP
nohup ./Ethernet_IPoIB_SSSP/PiGraph_IPoIB_SSSP -m $mutex -f $file -n $host_num -s $superstep -N ipoib -p $partition_opt -d $source_id 1 > execution_log/IPoIB_SSSP_$file_name.log_$host_num 2>&1
sleep 60s
nohup ./Ethernet_IPoIB_SSSP/PiGraph_IPoIB_SSSP -m $mutex -f $file -n $host_num -s $superstep -N ipoib -p $partition_opt -d $source_id 1 > check_resource_execution_log/IPoIB_SSSP_$file_name.log_$host_num 2>&1 &
pidof PiGraph_IPoIB_SSSP > pid
IPoIB_SSSP_pid=`cat pid`

nohup  pidstat -I -p $IPoIB_SSSP_pid 1 > test_log/cpu/IPoIB_SSSP_$file_name.log_cpu_$host_num 2>&1 &
nohup pidstat -r -p $IPoIB_SSSP_pid 1 > test_log/mem/IPoIB_SSSP_$file_name.log_mem_$host_num 2>&1 &

while :
do
    pidof PiGraph_IPoIB_SSSP > pid
    check_pid=`cat pid`
    if [ "$check_pid" == "" ]
    then
        echo IPoIB SSSP end
        break
    fi
    
done

sleep 60s