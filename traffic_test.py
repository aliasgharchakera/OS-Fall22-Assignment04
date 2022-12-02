import os
import random

Tests = 300

TimeOut = 5
MaxToa = 200
MaxToq = 200
MaxStd = 13
MinStd = 3

print("Path to exe (hit enter to default './a.out'): ", end='')
binary = input()
if binary == "":
	binary = "./a.out"

print("Enter USAGE_LIMIT (hit enter to default 7): ", end='')
USAGE_LIMIT = input()
if USAGE_LIMIT != '':
	USAGE_LIMIT = int(USAGE_LIMIT)
else:
	USAGE_LIMIT = 7

print("Enter ALLOWED_CARS (hit enter to default 3): ", end='')
ALLOWED_CARS = input()
if ALLOWED_CARS != '':
	ALLOWED_CARS = int(ALLOWED_CARS)
else:
	ALLOWED_CARS = 3

print("\nCreating and Running", Tests, "random tests plz wait")

def do_random_test(i):
    global MaxToa, MaxToq, MaxStd, MinStd, USAGE_LIMIT, binary, ALLOWED_CARS
    remaining_cars = []
    f = open("INPUT.txt", "w")
    std = random.randint(0, MaxStd-MinStd) + MinStd
    for j in range(std):
        remaining_cars.append(j)
        toa = random.randint(0, MaxToa)
        toq = random.randint(0, MaxToq)
        if random.randint(0, 20) % 2 == 0:
            f.write(str(toa) + ' ' + str(toq) + ' Outgoing\n')
        else:
            f.write(str(toa) + ' ' + str(toq) + ' Incoming\n')
    f.close()
    os.system("timeout "+str(TimeOut)+"s " + binary + " INPUT.txt > OUTPUT.txt")
    f = open("OUTPUT.txt", "r")
    output = f.readlines()
    if len(output) > 2:
        if output[-1].split()[0] != "Traffic":
            print("error: program not terminating! Perhaps stucked for infinity\n replace sleep() functions with usleep() to speed up")
            exit()
    else:
        print("error: program not terminating! Perhaps stucked for infinity\n replace sleep() functions with usleep() to speed up")
        exit()

    entries = 0
    street_cars = []
    since_repair = USAGE_LIMIT
    traffic_direction = "NAN"
    test_result = 0
    for i in range(1, len(output)):
        sentence = output[i]
        sentence = sentence.split()
        
        # print(i, sentence, len(sentence), traffic_direction, entries, since_repair)
        if sentence[0] == 'Traffic':
            if len(remaining_cars) > 0:
                print("error : simulation terminated before all cars were done")
                test_result = 1
            break
        if sentence[4] == 'travelled' and sentence[2] not in street_cars:
            print("error : car", sentence[2], "leaves before entering")
            test_result = 1
            break
        if sentence[4] == 'travelled':
            if int(sentence[2]) not in remaining_cars:
                print("error : car", sentence[2], "is unidentified")
                break
        if entries < 0 or entries > ALLOWED_CARS:
            print("error : #cars on street are too many or negative")
            test_result = 1
            break
        if since_repair == USAGE_LIMIT and sentence[4] == 'entered':
            print("error : street overused")
            test_result = 1
            break
        if sentence[4] == 'entered' and traffic_direction != sentence[0] and entries > 0:
            print("error : incoming and outgoing traffic at the same time")
            test_result = 1
            break
        if sentence[1] == 'street' and since_repair != USAGE_LIMIT :
            print("error : street repairing too early")
            test_result = 1
            break

        if sentence[4] == 'entered':
            if entries == 0:
                traffic_direction = sentence[0]
            since_repair = since_repair + 1
            entries = entries + 1
            street_cars.append(sentence[2])
        elif sentence[4] == 'travelled':
            remaining_cars.remove(int(sentence[2]))
            entries = entries - 1
            street_cars.remove(sentence[2])
        elif sentence[1] == 'street':
            since_repair = 0

    if entries != 0 and test_result == 0:
        print("error: total cars entered is not equal to total cars left")
        test_result = 1

    f.close()
    return test_result

for i in range(Tests):
    if do_random_test(i) == 1:
        print("Some rule was violated for INPUT.txt :( check your OUTPUT.txt")
        exit()
    else:
        #print("Test", i, "cleared")
        pass 

print("All", Tests, "random tests passed!")

