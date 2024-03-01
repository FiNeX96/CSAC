from time import time


MAX_N = 40
numbers = []
sum_count = []
count = []



def main():
    global numbers
    global sum_count
    global count
    
    count = [0] * (temp_max+1)
    count[0] = 1
    sum_count = [0] * (2*temp_max+1)   
    numbers = [0] * (temp_max+1)
     
    # count the rest
    start_time = time()
    recurse(1, 0)
    end_time = time()
    total_time = end_time - start_time
    # print the result
    for i in range(0, temp_max+1):
        # if i>0:
            # count[i] += count[i-1]
        print(f"# {i} {count[i]}")
    # Print to file
    # with open("count_sum_free_sets.txt", "a+") as file:
    #     file.write(f"{temp_max} {count[temp_max]} {total_time:e} seconds\n")
    # Print to console
    # print(f"{temp_max} {count[i]} {total_time:e} seconds\n")
    print("\n\n")

def recurse(first_try: int, set_size: int):
    while first_try <= temp_max:
        if sum_count[first_try] == 0:
            # count it
            count[first_try] += 1
            # update the set elements
            numbers[set_size] = first_try
            # count new sums
            for i in range(0, set_size+1):
                sum_count[first_try + numbers[i]] += 1
            # try one more
            recurse(first_try+1, set_size+1)    
            # uncount sums
            for i in range(0, set_size+1):
                sum_count[first_try + numbers[i]] -= 1                
        # update vars
        first_try += 1        
    
if __name__ == "__main__":
    global temp_max
    for i in range(1, MAX_N+1):
        temp_max = i
        main()
        