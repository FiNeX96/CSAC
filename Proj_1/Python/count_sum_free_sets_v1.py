import time

max_n = 60

def recursive(first_to_try, set_size):
    
    while (first_to_try < temp_max):
        
        if(sum_count[first_to_try] == 0):
            count[first_to_try] += 1
            
            a[set_size] = first_to_try
            
            for i in range(set_size+1):
                sum_count[a[i] + first_to_try] += 1
            
            recursive(first_to_try + 1, set_size +1)
            
            for i in range(set_size+1):
                sum_count[a[i] + first_to_try] -= 1
            
        first_to_try+=1
                

if __name__ == "__main__":
    
    
    for i in range(1, max_n + 1):
        temp_max = i
        a = []
        sum_count = []
        count = []
        count.append(1)
        temp_max = 0
        start_time = time.time()
        recursive(1,0)
        end_time = time.time()
        for j in range(0, temp_max):
            if (j > 0):
                count[j] += count[j - 1]
        print(len(count))
        print(i,count[-1]+1 , end_time - start_time)
    