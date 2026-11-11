def optimal_replacements(trace, page_size=4096, frames=2):
    memory = []
    faults = 0
    replacements = 0

    for i, (op, address) in enumerate(trace):
        page = address // page_size

        if page in memory:
            continue
        
        print(i,end=" ")
        print("Memory frames:", [f"{page:05x}" for page in memory], end=' ')
        faults += 1
        
        if len(memory) < frames:
            memory.append(page)
            continue

        future = [
            p // page_size
            for _, p in trace[i+1:]
        ]

        next_use = []
        for resident in memory:
            try:
                next_use.append(future.index(resident))
            except ValueError:
                next_use.append(float('inf'))

        victim = memory[next_use.index(max(next_use))]
        i = memory.index(victim)
        memory.insert(i, page)
        memory.remove(victim)
        replacements += 1
        print("Memory frames:", [f"{page:05x}" for page in memory],end="\n")
    return faults, replacements


trace = []

with open("test.trace", "r") as file:
    for line in file:
        parts = line.split()
        if len(parts) == 2:
            op, address = parts
            trace.append((op, int(address, 16)))

faults, replacements = optimal_replacements(trace)

print("Page faults:", faults)
print("Memory replacements:", replacements)