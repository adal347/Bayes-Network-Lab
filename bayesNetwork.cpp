class Node():
    """docstring for node."""
    parents = []
    probability_table = []
    name = ""
    def __init__(self, name):
        self.name = name


if __name__ == '__main__':

    section = 0
    inp = []
    nodes = []
    while section < 3:
        bfer = input()

        if bfer == "[Nodes]":
            name = input().split(",")
            print(str(name))
            while name:
                new_node = Node(name.pop())
                nodes.append(new_node)
            section += 1
        elif bfer == "[Probabilities]":
            print("Probabilities")
            section += 1
        elif bfer == "[Queries]":
            print("Queries")
            section += 1
