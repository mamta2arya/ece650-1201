#!/usr/bin/env python3
# coding: utf-8

import sys
import re


def print_out(line):
    print(line, file=sys.stdout)


def print_err(line):
    print(line, file=sys.stderr)


def point_to_float(point):
    return (float(point[0]), float(point[1]))


def is_on_line_segment(l_x1, l_y1, l_x2, l_y2, x, y):
    if (((min(l_x1, l_x2) <= x) and (x <= max(l_x1, l_x2)))
            and (min(l_y1, l_y2) <= y and (y <= max(l_y1, l_y2)))):
        return True
    else:
        return False


def get_intersection_of_two_lines(l1_x1, l1_y1, l1_x2, l1_y2, l2_x1, l2_y1, l2_x2, l2_y2):
    # for line 1
    a1 = l1_y2 - l1_y1
    b1 = l1_x1 - l1_x2
    c1 = a1 * l1_x1 + b1 * l1_y1

    # for line 2
    a2 = l2_y2 - l2_y1
    b2 = l2_x1 - l2_x2
    c2 = a2 * l2_x1 + b2 * l2_y1

    # determinant of denominator
    denominator = a1 * b2 - a2 * b1

    if int(denominator) == 0:
        # lines are parallel, cover each edge cases related to them

        if l1_x1 == l2_x1 and l1_y1 == l2_y1 and l1_x2 == l2_x2 and l1_y2 == l2_y2:
            return (l1_x1, l1_y1)
        if is_on_line_segment(l1_x1, l1_y1, l1_x2, l1_y2, l2_x1, l2_y1):
            return (l2_x1, l2_y1)
        if is_on_line_segment(l1_x1, l1_y1, l1_x2, l1_y2, l2_x2, l2_y2):
            return (l2_x2, l2_y2)
        if is_on_line_segment(l2_x1, l2_y1, l2_x2, l2_y2, l1_x1, l1_y1):
            return (l1_x1, l1_y1)
        if is_on_line_segment(l2_x1, l2_y1, l2_x2, l2_y2, l1_x2, l1_y2):
            return (l1_x2, l1_y2)

        if (l1_x2, l1_y2) == (l2_x1, l2_y1) and (l1_x1, l1_y1) != (l2_x2, l2_y2) \
                or (l1_x1, l1_y1) == (l2_x1, l2_y1) and (l1_x2, l1_y2) != (l2_x2, l2_y2):
            return (l2_x1, l2_y1)

        if (l1_x1, l1_y1) == (l2_x2, l2_y2) and (l1_x2, l1_y2) != (l2_x1, l2_y1) \
                or (l1_x2, l2_y2) == (l2_x2, l2_y2) and (l1_x1, l1_y1) != (l2_x1, l2_y1):
            return (l2_x2, l2_y2)

    else:
        x = (b2*c1 - b1*c2)/denominator
        y = (a1*c2 - a2*c1)/denominator
        # to check x, y are on segment of both lines
        if is_on_line_segment(l1_x1, l1_y1, l1_x2, l1_y2, x, y) and \
                is_on_line_segment(l2_x1, l2_y1, l2_x2, l2_y2, x, y):
            return (round(x, 2), round(y, 2))

    return None


def get_intersections_data_of_two_streets(street1, street2):
    s1 = store.get(street1, [])
    s2 = store.get(street2, [])

    # a list of tuples, each tuple will store intersecton and lines related to
    # that intersection
    intersections_data = []

    for l1_p1, l1_p2 in zip(s1, s1[1:]):
        for l2_p1, l2_p2 in zip(s2, s2[1:]):

            # coerce each point coordinates to float, if not
            l1_p1 = point_to_float(l1_p1)
            l1_p2 = point_to_float(l1_p2)
            l2_p1 = point_to_float(l2_p1)
            l2_p2 = point_to_float(l2_p2)

            intersection = get_intersection_of_two_lines(
                l1_p1[0], l1_p1[1],  # line1 start point
                l1_p2[0], l1_p2[1],  # line1 end point
                l2_p1[0], l2_p1[1],  # line2 start point
                l2_p2[0], l2_p2[1],  # line2 endpoint
            )
            if intersection is None:
                pass
            else:
                intersection = point_to_float(intersection)

                intersections_data.append(
                    (intersection,
                     [l1_p1, l1_p2],  # line1
                     [l2_p1, l2_p2],  # line2
                     )
                )
    return intersections_data


def get_pairs(arr):
    result = []
    for p1 in range(len(arr)):
        for p2 in range(p1+1, len(arr)):
            result.append([arr[p1], arr[p2]])
    return result


def get_all_vertices_from_intersections(intersections_data):
    vertices = []
    for interection_data in intersections_data:
        points = []
        points.append(interection_data[0])  # add intersection point
        points.append(interection_data[1][0])  # add line 1 point 1
        points.append(interection_data[1][1])  # add line 1 point 2
        points.append(interection_data[2][0])  # add line 2 point 1
        points.append(interection_data[2][1])  # add line 2 point 2

        for point in points:
            if point not in vertices:
                vertices.append(point)
    return vertices


def get_all_edges_from_intersections(intersections_data):
    # a list of tuples, each tuple will contain id of vertices related to edge
    edges = []
    for intersection_data in intersections_data:
        intersection_edges = []
        # add edges related to line 1 of intersection
        intersection_edges.append(
            [intersection_data[1][0], intersection_data[0]])
        intersection_edges.append(
            [intersection_data[0], intersection_data[1][1]])

        # add edges related to line 2 of intersection
        intersection_edges.append(
            [intersection_data[2][0], intersection_data[0]])
        intersection_edges.append(
            [intersection_data[0], intersection_data[2][1]])

        for intersection_edge in intersection_edges:
            vertex1 = intersection_edge[0]
            vertex2 = intersection_edge[1]

            vertex1_id = get_index_of_vertex(vertex1)
            vertex2_id = get_index_of_vertex(vertex2)
            
            if vertex1_id is not None and vertex2_id is not None:
                edges.append((vertex1_id, vertex2_id))
    return edges


def print_graph(vertices, edges):
    print_out('V = {')
    for index, vertex in enumerate(vertices):
        print_out(f'  {index}:  ({vertex[0]:.2f}, {vertex[1]:.2f})')
    print_out('}')
    print_out('E = {')
    for edge in edges:
        print_out(f'  <{edge[0]}, {edge[1]}>,')
    print_out('}')


def get_street_name_from_line(line, command, command_format):
    # line_new = ' "Weber Street" (2,-1) (2,2) (5,5) (5,6) (3,8)'
    line_new = line[1:]

    if not line_new:
        print_out(
            f"Error: Not found a street name in command, valid format for "
            f"'{command}' command is '{command_format}'"
        )
        return None

    if line_new[0] != ' ':
        print_out(
            f"Error: Street name should be seperated by whitespace after command"
            f" character, valid format for '{command}' command is '{command_format}'"
        )
        return None

    line_new = line_new.strip()

    if not line_new:
        print_out(
            f"Error: Not found a street name in command, valid format for "
            f"'{command}' command is '{command_format}'"
        )
        return None

    if line_new[0] != '"' or '"' not in line_new[1:]:
        print_out(
            f"Error: Street name in command should be enclosed in double quotes,"
            f" valid format for '{command}' command is '{command_format}'"
        )
        return None

    # assume there is a stree name in line, and extract it
    line_split = line_new.split('"', maxsplit=2)
    street_name = line_split[1]
    street_name = street_name.lower()
    return street_name

def get_vertices_from_line(line, command, command_format):
    line_split = line.rsplit('"', maxsplit=1)

    street_vertices_str = line_split[1].strip()
    if not street_vertices_str:
        print_out(
            f"Error: Not found any vertices for a street '{street_name}'"
            f" in command, valid format for '{command}' command is '{command_format}'"
        )
        return (None, None)

    street_vertices = []

    open_brace_count = street_vertices_str.count('(')
    close_brace_count = street_vertices_str.count(')')
    if open_brace_count != close_brace_count:
        print_out(
            f"Error: Invalid format for '{command}' command"
        )
        return (None, None)
    
    street_vertices_str = street_vertices_str.replace(" ", "")
    street_vertices_str = street_vertices_str.replace(")(", ",")
    street_vertices_str = street_vertices_str.replace(")", "")
    street_vertices_str = street_vertices_str.replace("(", "")
    street_vertices_str = street_vertices_str.replace(",", " ")

    vertices = []
    street_vertices_str = street_vertices_str.split(' ')
    vertices = [x for x in street_vertices_str if x != ""]
    
    vertex_coor_even = vertices[0::2]
    vertex_coor_odd = vertices[1::2]
    
    if len(vertex_coor_even) != len(vertex_coor_odd):
        print_out(
            f"Error: Invalid format for '{command}' command"
        )
        return (None, None)
    

    for i in range(len(vertex_coor_even)):
        street_vertices.append((float(vertex_coor_even[i]), float(vertex_coor_odd[i])))
 
    return street_vertices


def get_street_name_and_vertices_from_line(line, command, command_format):
    # extract street name first
    street_name = get_street_name_from_line(line, command, command_format)

    if not street_name:
        return (None, None)

    # extract street vertices
    street_vertices = get_vertices_from_line(line, command, command_format)

    if not street_vertices or len(street_vertices) < 2:
        # not a valid format for vertices, we got empty list
        print_out(
            f"Error: Required atleast two vertices for a street and each should be"
            f" seperated by one whitespace character, valid format for"
            f" '{command}' command is '{command_format}'"
        )
        return (None, None)
    
    return street_name, street_vertices


def process_add_street_command(line):
    street_name, street_vertices = get_street_name_and_vertices_from_line(
        line, 'a', 'a "street name" (x1, y1) (x2, y2)')

    if not street_name or not street_vertices:
        return

    if street_name in store:
        print_err(
            f"Error: Street '{street_name}' already exists, please use 'c'"
            f" command to change or 'r' command to remove ."
        )
    else:
        store[street_name] = street_vertices


def processs_change_street_command(line):
    street_name, street_vertices = get_street_name_and_vertices_from_line(
        line, 'c', 'c "street name" (x1, y1) (x2, y2)')

    if not street_name or not street_vertices:
        return

    if street_name in store:
        # change command
        store[street_name] = street_vertices
    else:
        print_err(
            f"Error: Street '{street_name}' does not exists, please use 'a'"
            f" command to add first ."
        )


def process_remove_street_command(line):
    street_name = get_street_name_from_line(line, 'r', 'r "street name"')
    
    if not street_name:
        return

    # check after street name
    line_split = line.rsplit('"', maxsplit=1)
    if line_split[1].strip():
        print_out(
            f"Error: Invalid format for 'r' command"
        )
        return

    if street_name in store:
       # remove street
        store.pop(street_name)
    else:
        print_err(
            f"Error: Street '{street_name}' does not exists, please use 'a'"
            f" command to add first ."
        )


def process_print_graph_command(line):
    # get pairs of streets
    streets = store.keys()
    street_pairs = get_pairs(list(streets))

    # find all the intersections across streets
    all_intersections = []
    for street1, street2 in street_pairs:
        all_intersections += get_intersections_data_of_two_streets(
            street1, street2)

    # find all valid vertices from intersections
    graph_vertices = get_all_vertices_from_intersections(all_intersections)

    # add these vertices to vertices store
    vertices_store.update(graph_vertices)
def get_index_of_vertex(vertex):

    # find all valid edges from intersections
    graph_edges = get_all_edges_from_intersections(all_intersections)

    # print graph
    print_graph(graph_vertices, graph_edges)


    if vertex in vertices_store:
        return list(vertices_store).index(vertex)
    return None

# create a empty store for storing street data
# each key will be stree name and value will be a list of tuples, each tuple
# will represent one vertex (x, y)
store = {}

# create a empty store for storing all unique vertices
# index of each vertex in store will be it's unqiue id which will be same
# throughout each execution of this program
vertices_store = set()


def main():
    while True:
        line = sys.stdin.readline()

        if line == '':
            break

        # extract the command
        command = line[0]

        if command == 'a':
            process_add_street_command(line)
        elif command == 'c':
            processs_change_street_command(line)
        elif command == 'r':
            process_remove_street_command(line)
        elif command == 'g':
            process_print_graph_command(line)
        else:
            print_err(
                f"Error: '{command}' is not a valid command, valid commands"
                f" are 'a', 'c', 'r' and 'g' ."
            )

if __name__ == "__main__":
    while True:
        try:
            main()
            sys.exit(0)
            break
        except Exception as e:
            # catch any type of error, so that our program will not quit and
            # keep running
            print_err(
                f"Error: Something went wrong ({e.__class__.__name__}: {str(e)}).")
