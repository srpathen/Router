import sys
import matplotlib.pyplot as plt
import matplotlib.patches as patches

from matplotlib.widgets import CheckButtons

colorWheel = ["pink", "red", "blue", "green", "purple", "orange", "yellow"]

def main():
    # Check if a filename was provided
    
    filename = sys.argv[1]
    width = float(sys.argv[2])
    height = float(sys.argv[3])

    try:
        # Open and read the file
        with open(filename, 'r') as file:
            content = file.read()
        metInfo, nodes, obstrs = parse_file(content)
        fig, ax = plt.subplots()

        vis = [True] * len(metInfo)
        vis_dict = dict(zip(metInfo.keys(), vis))
        

        plotGuides(ax, width, height, metInfo, vis_dict)
        plotNodes(ax, nodes, vis_dict, metInfo)
        plotObstrs(ax, obstrs, vis_dict, metInfo)
        ax_checkbox = plt.axes([0, 0, 0.1, 0.1])
        ax.set_xlim(0, width)
        ax.set_ylim(0, height)
        ax.set_aspect('equal')
        
        check = CheckButtons(ax_checkbox, metInfo.keys(), vis)

        def update(label):
            vis_dict[int(label)] = not vis_dict[int(label)]
            ax.cla()
            plotGuides(ax, width, height, metInfo, vis_dict)
            plotNodes(ax, nodes, vis_dict, metInfo)
            plotObstrs(ax, obstrs, vis_dict, metInfo)
            ax.set_xlim(0, width)
            ax.set_ylim(0, height)
            ax.set_aspect('equal')
            plt.draw()

        check.on_clicked(update)

        plt.show()
    except FileNotFoundError:
        print(f"Error: The file '{filename}' was not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

def plotGuides(ax, winwidth, winheight, metInfo, vis_dict):
    for layer in metInfo:
        if not vis_dict[layer]:
            continue
        if metInfo[layer]["orient"] == "VERTICAL":
            for i in range(metInfo[layer]["numtrk"]):
                coordinate = (metInfo[layer]["offset"] + i * (metInfo[layer]["inc"] + metInfo[layer]["width"]),0)
                width =  metInfo[layer]["width"]
                height = winheight
                facecolor = colorWheel[metInfo[layer]["layer"]]
                rect = patches.Rectangle(coordinate, width, height, facecolor=facecolor, alpha=0.1)
                ax.add_patch(rect)
        elif metInfo[layer]["orient"] == "HORIZONTAL":
            for i in range(metInfo[layer]["numtrk"]):
                coordinate = (0,metInfo[layer]["offset"] + i * (metInfo[layer]["inc"] + metInfo[layer]["width"]))
                width = winwidth
                height = metInfo[layer]["width"]
                facecolor = colorWheel[metInfo[layer]["layer"]]
                rect = patches.Rectangle(coordinate, width, height, facecolor=facecolor, alpha=0.1)
                ax.add_patch(rect)

def plotNodes(ax, nodes, vis_dict, metInfo):
    max_metal = len(metInfo)
    for node in nodes:
        if node != None:
            if vis_dict[node["node"]["met"]]:
                if node["north"] != None:
                    ax.plot([node["node"]["coord"][0], node["north"]["coord"][0]], 
                            [node["node"]["coord"][1], node["north"]["coord"][1]], 
                            marker="x", color=colorWheel[node["node"]["met"]])
                if node["south"] != None:
                    ax.plot([node["node"]["coord"][0], node["south"]["coord"][0]], 
                            [node["node"]["coord"][1], node["south"]["coord"][1]], 
                            marker="x", color=colorWheel[node["node"]["met"]])
                if node["east"] != None:
                    ax.plot([node["node"]["coord"][0], node["east"]["coord"][0]], 
                            [node["node"]["coord"][1], node["east"]["coord"][1]], 
                            marker="x", color=colorWheel[node["node"]["met"]])
                if node["west"] != None:
                    ax.plot([node["node"]["coord"][0], node["west"]["coord"][0]], 
                            [node["node"]["coord"][1], node["west"]["coord"][1]], 
                            marker="x", color=colorWheel[node["node"]["met"]])

def plotObstrs(ax, obstrs, vis_dict, metInfo):
    for obstr in obstrs:
        metSpacing = metInfo[obstr["layer"]]["mspa"]
        rect = patches.Rectangle(obstr["coord"], obstr["width"], obstr["height"], facecolor=colorWheel[obstr["layer"]], alpha=0.5)
        ax.add_patch(rect)
        rect = patches.Rectangle((obstr["coord"][0]-metSpacing,obstr["coord"][0]-metSpacing), obstr["width"]+2*metSpacing, obstr["height"]+2*metSpacing, 
                                 facecolor=colorWheel[obstr["layer"]], alpha=0.2)
        ax.add_patch(rect)

def parseNode(node):
    if node != "NULL":
        numbers = node[1:-1].split(",")
        return {"coord": (int(numbers[0]), int(numbers[1])), "met": int(numbers[2])}
    return None

def parse_file(file_content):
    metDict = dict()
    nodes = []
    obstrs = []
    lines = file_content.split("\n")
    for line in lines:
        contents = line.split(" ")
        if not len(contents):
            continue
        if contents[0] == "metinfo":
            metinfo = dict()
            metinfo["layer"] = int(contents[1])
            metinfo["orient"] = contents[2]
            metinfo["offset"] = int(contents[3])
            metinfo["inc"] = int(contents[4])
            metinfo["width"] = int(contents[5])
            metinfo["numtrk"] = int(contents[6])
            metinfo["mspa"] = int(contents[7])
            metDict[int(contents[1])] = metinfo
        elif contents[0] == "node":
            nodeInfo = dict()
            nodeInfo["node"] = parseNode(contents[1])
            nodeInfo["up"] = parseNode(contents[2])
            nodeInfo["down"] = parseNode(contents[3])
            nodeInfo["north"] = parseNode(contents[4])
            nodeInfo["east"] = parseNode(contents[5])
            nodeInfo["south"] = parseNode(contents[6])
            nodeInfo["west"] = parseNode(contents[7])
            nodes.append(nodeInfo)
        elif contents[0] == "obstruction":
            obstr = dict()
            obstr["coord"] = (int(contents[1][1:-1].split(",")[0]), int(contents[1][1:-1].split(",")[1]))
            obstr["width"] = int(contents[2])
            obstr["height"] = int(contents[3])
            obstr["layer"] = int(contents[4])
            obstrs.append(obstr)
    return (metDict, nodes, obstrs)

if __name__ == "__main__":
    main()