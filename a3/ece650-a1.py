#!/usr/bin/env python3
import sys
import re


# YOUR CODE GOES HERE
class Graph:
    def __init__(self):
        self.index = 1
        self.nodes = []
        self.streets = []
        self.E = []

    # for a graph to add a node(vertex)
    def add_node(self, x, y, parent_street, related_streets):
        new_node = Node(x, y, self.index, parent_street)
        existing_node = None

        for node in self.nodes:
            if node.is_same_point(new_node):
                existing_node = node
                break

        if existing_node:
            for street in related_streets:
                # Update the existing node's related streets
                if street not in existing_node.related_streets:
                    existing_node.related_streets.append(street)
            return existing_node.label
        else:
            # Create a new node and set the related street
            self.nodes.append(new_node)
            new_node.related_streets = related_streets
            self.index += 1
            return new_node.label

    # function to update_E
    def update_E(self):
        self.E = []
        edges = []

        for street in self.streets:
            for edge in street.edges:
                edge.has_nodes = []
                edges.append(edge)

        for node in self.nodes:
            for edge in edges:
                if node_on_edge(node, edge):
                    edge.has_nodes.append(node)

        for edge in edges:
            for node in edge.has_nodes:
                if node.parent_street is None:
                    nodes_except_intersect = []
                    for another_node in edge.has_nodes:
                        if another_node != node:
                            nodes_except_intersect.append(another_node)
                    node1, node2 = find_points_in_middle(nodes_except_intersect, node)
                    self.E.append((node1.label, node.label))
                    self.E.append((node2.label, node.label))

        # remove redundant E eg: (1->2) is the same with (2->1)
        redundant_E_to_remove = []
        for i in range(len(self.E)):
            for j in range(i + 1, len(self.E)):
                if self.E[j][0] == self.E[i][1] and self.E[j][1] == self.E[i][0]:
                    redundant_E_to_remove.append(self.E[j])
        for redundant_E in redundant_E_to_remove:
            self.E.remove(redundant_E)

    def add_street(self, street_name, coordinates):
        new_street = Street(street_name, coordinates)

        # find intersections when there are more than 1 street
        if len(self.streets) > 0:
            for street in self.streets:
                for edge1 in street.edges:
                    for edge2 in new_street.edges:
                        intersection = line_intersection(
                            (edge1.start, edge1.end), (edge2.start, edge2.end)
                        )
                        if intersection is not None:
                            # add two edges' endpoints to node list if intersect
                            self.add_node(edge1.start[0], edge1.start[1], street, [new_street])
                            self.add_node(edge1.end[0], edge1.end[1], street, [new_street])
                            self.add_node(edge2.start[0], edge2.start[1], new_street, [street])
                            self.add_node(edge2.end[0], edge2.end[1], new_street, [street])
                            # add the intersection point to node list
                            self.add_node(intersection[0], intersection[1], None, [street, new_street])

        # add the street to the graph's street list
        self.streets.append(new_street)

    def mod_street(self, street_name, coordinates):
        for street in self.streets:
            if street_name == street.name:
                # remove the street and add the street
                self.rm_street(street_name)
                self.add_street(street_name, coordinates)
                return True
        print("Error: Street does not exist!")

    def rm_street(self, street_name):
        # return false if street not found
        if not any(street.name == street_name for street in self.streets):
            print("Error: Street does not exist!")
            return False

        # Find streets to remove
        for street in self.streets:
            if street_name == street.name:
                self.streets.remove(street)
                break

        nodes_to_remove = []
        # Find nodes related to streets to remove
        for node in self.nodes:
            if node.parent_street is None:  # it is an intersection if parent street is None
                for related_street in node.related_streets:
                    # if this intersection has related street user want to remove
                    if street_name == related_street.name:
                        # if this intersection intersected by two streets
                        if len(node.related_streets) < 3:
                            nodes_to_remove.append(node)
                            break
                        # if this intersection intersected by more than two streets
                        else:
                            node.related_streets.remove(related_street)
                            break
            elif node.parent_street.name == street_name:  # if this node's parent street is the removing street
                nodes_to_remove.append(node)
            else:  # if  this node is another street's point but related to removing street
                for related_street in node.related_streets:
                    if street_name == related_street.name:
                        if len(node.related_streets) == 1:
                            nodes_to_remove.append(node)
                            break
                        else:
                            node.related_streets.remove(related_street)

        # remove all the nodes need to be removed
        for node in nodes_to_remove:
            self.nodes.remove(node)


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y


class Node(Point):
    def __init__(self, x, y, label, parent_street):
        self.x = x
        self.y = y
        self.label = label
        self.parent_street = parent_street
        self.related_streets = []

    def get_label(self):
        return self.label

    def is_same_point(self, node):
        if self.x == node.x and self.y == node.y:
            return True
        else:
            return False


class Edge:
    def __init__(self, start, end):
        self.start = start
        self.end = end
        self.has_nodes = []

    def intersect(self, edge):
        line1 = [self.start, self.end]
        line2 = [edge.start, edge.end]
        return line_intersection(line1, line2)


class Street:
    def __init__(self, street_name, coordinates):
        self.name = street_name
        self.edges = []

        # add the edges
        for i in range(len(coordinates) - 1):
            edge = Edge(coordinates[i], coordinates[i + 1])
            self.edges.append(edge)


def det(a, b):
    return a[0] * b[1] - a[1] * b[0]


# function to determine whether two line intersect and return the intersection point
def line_intersection(line1, line2):
    x1, y1 = line1[0]
    x2, y2 = line1[1]
    x3, y3 = line2[0]
    x4, y4 = line2[1]

    # Calculate the direction vectors of the two line segments
    dx1 = x2 - x1
    dy1 = y2 - y1
    dx2 = x4 - x3
    dy2 = y4 - y3

    # Calculate the determinant
    det = dx1 * dy2 - dx2 * dy1

    # Check if the line segments are parallel (det == 0)
    if det == 0:
        return None  # Lines are parallel and do not intersect

    # Calculate the parameters for the line equations
    t1 = ((x3 - x1) * dy2 - (y3 - y1) * dx2) / det
    t2 = ((x3 - x1) * dy1 - (y3 - y1) * dx1) / det

    # Check if the intersection point is within the line segments
    if 0 <= t1 <= 1 and 0 <= t2 <= 1:
        x = x1 + t1 * dx1
        y = y1 + t1 * dy1
        return x, y  # Intersection point within line segments

    return None


# function to find out if a node is on the given edge
def node_on_edge(node, edge):
    a = edge.start[0]
    c = edge.end[0]
    b = edge.start[1]
    d = edge.end[1]
    e = node.x
    f = node.y
    if min(a, c) <= e <= max(a, c) and min(b, d) <= f <= max(b, d):
        if a == c:
            return e == a
        else:
            m = (d - b) / (c - a)
            y_on_line = m * (e - a) + b
            return y_on_line == f
    else:
        return False


# calculate the distance between two nodes
def calculate_distance(node1, node2):
    x1 = node1.x
    y1 = node1.y
    x2 = node2.x
    y2 = node2.y
    dx = x1 - x2
    dy = y1 - y2
    distance = (dx ** 2 + dy ** 2) ** 0.5
    return distance


# find the two points in the line_segment that the given point is located between.
def find_points_in_middle(line_segment, node):
    if len(line_segment) < 2:
        raise ValueError("A line segment must have at least two points.")

    closest_points = []
    closest_distance = float('inf')
    for i in range(len(line_segment)):
        for j in range(i + 1, len(line_segment)):
            # Ensure p1 is the leftmost point
            if line_segment[i].x > line_segment[j].x:
                line_segment[i], line_segment[j] = line_segment[j], line_segment[i]
            # Calculate the distances from the point to the two endpoints of the segment
            dist1 = calculate_distance(node, line_segment[i])
            dist2 = calculate_distance(node, line_segment[j])
            if dist1 + dist2 < closest_distance:
                closest_distance = dist1 + dist2
                closest_points = [line_segment[i], line_segment[j]]
    if closest_points:
        return closest_points[0], closest_points[1]
    else:
        return None, None


def main():
    # YOUR MAIN CODE GOES HERE
    graph = Graph()
    # sample code to read from stdin.
    # make sure to remove all spurious print statements as required
    # by the assignment
    while True:
        line = sys.stdin.readline()
        if line == "":
            break
        else:
            command_format = r'(add|mod|rm|gg)((?: +?"[a-zA-Z ]+?" *?))?((?:\([-]?[0-9]+?,[-]?[0-9]+?\) *?)*)?$'
            match = re.match(command_format, line)
            if match:
                command = match.group(1)
                street_name = match.group(2)
                if street_name:
                    street_name = street_name.split("\"")[1]
            else:
                print("Error: Your input is not in valid format")
                continue

            if command == "add" or command == "mod":
                # read the coordinates
                coordinate = re.compile(r'\((-?\d+),(-?\d+)\)')
                input_coordinates = re.findall(coordinate, line)
                coordinates = []
                for i in range(len(input_coordinates)):
                    new_coordinate = (float(input_coordinates[i][0]), float(input_coordinates[i][1]))
                    coordinates.append(new_coordinate)

                if command == "add":
                    # add the street
                    graph.add_street(street_name, coordinates)

                elif command == "mod":
                    # modify the street
                    graph.mod_street(street_name, coordinates)

            elif command == "rm":
                # remove the street
                graph.rm_street(street_name)

            elif command == "gg":
                print("V = {")
                for node in graph.nodes:
                    print("\t%d: (%s, %s)" % (node.label, round(node.x, 3), round(node.y, 3)))
                print("}")
                graph.update_E()
                print("E = {")
                for E in graph.E:
                    print("\t<%d, %d>," % (E[0], E[1]))
                print("}")
        print("read a line:", line)

    print("Finished reading input")
    # return exit code 0 on successful termination
    sys.exit(0)


if __name__ == "__main__":
    main()
