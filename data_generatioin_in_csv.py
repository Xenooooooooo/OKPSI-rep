import random

output_path_r_8 = r'./out/build/linux/frontend/test_r_8.csv'                     # path for receiver's dataset
output_path_s_8 = r'./out/build/linux/frontend/test_s_8.csv'                     # path for sender's dataset
output_path_intersec_8 = r'./out/build/linux/frontend/test_intersec_8.csv'       # path for intersection

temp_r = set()
temp_s = set()
intersection = set()

intersection_card = 200000              # the cardinality of the intersection
data_max_value = 9999999999999999       # 16 in deci
data_number = 1000000                   # The number is 
add_in_threshold = 85                   # the threshold of probability in selecting intersection values

with open(output_path_intersec_8, mode = 'w') as csv_intersec_8:
    with open(output_path_s_8, mode = 'w') as csv_s_8:
        with open(output_path_r_8, mode = 'w') as csv_r_8:
            count = 0
            for i in range(data_number):        # build receiver's dataset
                a = random.randint(0, data_max_value)
                while a in temp_r: a = random.randint(0, data_max_value)
                temp_r.add(a)
                print(str(a).zfill(16), file = csv_r_8) 

                b = random.randint(0, 100)          # generate the probability
                if count < intersection_card:       # if the intersection hasn't full yet
                    if b > add_in_threshold:        # if add this value into sender's dataset
                        print(str(a).zfill(16), file = csv_s_8)
                        print(str(a).zfill(16), file = csv_intersec_8)
                        count += 1
                        # intersection.add(b)       # if the intersection is needed in the code, then activate this line
                if i % 1000000 == 0: print(str(i) + "/100000000, " + str(i / data_number))  # progress bar

            for i in range(data_number - intersection_card):    # build sender's dataset upon the intersection
                a = random.randint(0, data_max_value)
                while a in temp_r: a = random.randint(0, data_max_value)
                temp_r.add(a)
                print(str(a).zfill(16), file = csv_s_8)
                if i % 1000000 == 0: print(str(i) + "/100000000, " + str(i / data_number))  # progress bar