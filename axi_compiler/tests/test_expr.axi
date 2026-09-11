state count: int = 0
state active: bool = count < 10

while active:
    count = count + 1
    if count == 5:
        print("Count is 5!")
    else:
        print(count)
    
    active = count < 10
