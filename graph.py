import matplotlib.pyplot as plt

plt.xlabel("fidelity");
plt.ylabel("success prob.")

with open('output.txt','r') as f:
    for line in f:
        data = line.split(" ")
        #print(data)
        plt.plot([float(data[0])],[float(data[1])],'ro')
plt.show()

