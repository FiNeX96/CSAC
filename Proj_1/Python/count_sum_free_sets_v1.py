from elapsed_time import cpu_time

max_n = 40
a = []
sum_count = []
count = []

def recursive(first_to_try, set_size):
    
    while (first_to_try; first_to_try < max_n; first_to_try += 1):
        
        if(sum_count[first_to_try] == 0):
            count[first_to_try] += 1
            
            a[set_size] = first_to_try
            
            for (i = 0; i <= set_size; i += 1):
                sum_count[a[i] + first_to_try] += 1
            
            recursive(first_to_try + 1, set_size +1)
            
            for (i = 0; i <= set_size; i += 1):
                sum_count[a[i] + first_to_try] -= 1
                

if __name__ == "__man__":
    count[0] = 1
    
    recursive(1, 0)
    
    for (i = 0; i <= max_n; i += 1):
        
        if (i > 0):
            count[i] += count[i - 1]
    
        print(f'{i}  -  {count[i]}')
    
    print(f'# {max_m} {cpu_time()}')