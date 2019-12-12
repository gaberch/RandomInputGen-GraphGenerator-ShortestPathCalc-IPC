#!/usr/bin/env python
from __future__ import print_function
import re
import sys

class CustomError (Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

def parse_input(input):

    # Set up check against spaces, special characters and numbers
    spaces_and_full_nums = re.compile(r'[(),]|[0-9]+')
    quotes = re.compile(r'["]')
    nums = re.compile(r'[-]?[0-9]+')
    space = re.compile(r'[^ ]')
    spec_math = re.compile(r'[@_!#$%^&*()<>?/\|}{~:",.+=-]')
    brackets_commas = re.compile(r'[(),]')
    invalid_pts_spec_char = re.compile(r'[@_!#$%^&*<>?/\|}{~:".+=]')
    letters = re.compile (r'([a-z]+)')

    # separate command components on input
    input_set = input.split('"')



    cmd = input_set[0].replace(" ", "")

    # Check input error on command
    if cmd not in ['a', 'c', 'g', 'r'] :
        raise CustomError("Invalid command")
    if input_set[0][0] == " ":
        raise CustomError("Space given before first command")

    if cmd == 'a' or cmd == 'c':

        # Check for sufficient quotations in command
        if len(quotes.findall(input)) != 2:
            raise CustomError("Invalid command")

        if input_set[0][-1] != " ":
            raise CustomError("No space between command and street name")
        elif input_set[2][0] != " ":
            raise CustomError("No space between street name and points")


        # Check input error on street name
        if len(input_set[1]) == 0 or len(space.findall(input_set[1])) == 0:
            raise CustomError("No street name given")
        elif len(spec_math.findall(input_set[1])) != 0 or len(nums.findall(input_set[1])) != 0:
            raise CustomError("Street name contains special characters or numbers")

        # Check for coordinate input errors

        lb_check = False
        x_check = False
        comma_check = False
        y_check = False
        rb_check = False

        for i in range(len(input_set[2])):
            if input_set[2][i] == "-" and len(nums.findall(input_set[2][i+1])) == 0:
                raise CustomError("Negative sign not next to a number. Check your points input.")

        points_no_spaces = spaces_and_full_nums.findall(input_set[2])

        for i in range(len(points_no_spaces)):

            if lb_check and x_check and comma_check and y_check and rb_check:
                lb_check = False
                x_check = False
                comma_check = False
                y_check = False
                rb_check = False

            if points_no_spaces[i] == "(":
                if lb_check:
                    raise CustomError("Invalid point input")
                lb_check = True

            elif points_no_spaces[i] in nums.findall(points_no_spaces[i]) and lb_check and not comma_check:
                if x_check:
                    raise CustomError("Invalid point input")
                x_check = True

            elif points_no_spaces[i] == ",":
                if comma_check:
                    raise CustomError("Invalid point input")
                comma_check = True
            elif points_no_spaces[i] in nums.findall(points_no_spaces[i]) and lb_check and comma_check and x_check:
                if y_check:
                    raise CustomError("Invalid point input")
                y_check = True
            elif points_no_spaces[i] == ")":
                if rb_check:
                    raise CustomError("Invalid point input")
                rb_check = True

            if x_check and not lb_check:
                raise CustomError("Invalid point input")
            elif comma_check and not(x_check and lb_check):
                raise CustomError("Invalid point input")
            elif y_check and not (comma_check and x_check and lb_check):
                raise CustomError("Invalid point input")
            elif rb_check and not (lb_check and x_check and comma_check and y_check):
                raise CustomError("Invalid point input")
            elif i+1 == len(points_no_spaces) and not rb_check:
                raise CustomError("Invalid point input")

        if len(invalid_pts_spec_char.findall(input_set[2])) != 0 or len(letters.findall(input_set[2])) != 0:
            raise CustomError("Points contain an invalid character")

        # record points

        pts = []
        street_name = input_set[1]
        coord = nums.findall(input_set[2])

        for i in range(len(coord)):
            coord[i] = int(coord[i])

        count = 0

        while count < len(coord):
            pts.append([coord[count],coord[count+1]])
            count += 2

        return cmd, street_name, pts

    elif cmd == 'r':

        # Check for sufficient quotations in command
        if len(quotes.findall(input)) != 2:
            raise CustomError("Invalid command")

        if input_set[0][-1] != " ":
            raise CustomError("No space between command and street name")
        # elif input_set[2][0] != " ":
        #     raise CustomError("No space between street name and points")

        # Check input error on street name
        if len(input_set[1]) == 0 or len(space.findall(input_set[1])) == 0:
            raise CustomError("No street name given")
        elif len(spec_math.findall(input_set[1])) != 0 or len(nums.findall(input_set[1])) != 0:
            raise CustomError("Street name contains special characters or numbers")

        # Check for coordinate input errors


        if len(space.findall(input_set[2])) != 0:
            raise CustomError("Invalid command")


        street_name = input_set[1]

        return cmd, street_name

    elif cmd == 'g':

        if len(input_set) > 1:
            count = 1
            while count < len(input_set):
                if len(space.findall(input_set[count])) != 0:
                    raise CustomError("Invalid command")
                count += 1

        return cmd


class StreetDB(object):

    def __init__(self):
        self.data = {}

    def __repr__(self):
        info = ""
        for key,value in self.data.iteritems():
            info += str(key) + ":" + str(value) + "\n"
        return info

    def add(self, new_street, coords):
        if len(coords) < 2:
            raise CustomError("Not enough points given")
        for key,value in self.data.iteritems():
            if new_street.lower() == key.lower():
                raise CustomError("Street already exists")

        self.data[new_street] = coords

    def change(self, street_name, coords):
        street_exists = False
        if len(coords) < 2:
            raise CustomError("Not enough points given")
        for key, value in self.data.iteritems():
            if street_name.lower() == key.lower():
                street_exists = True
                self.data[key] = coords
        if not street_exists:
            raise CustomError("Street doesn't exist")

    def remove(self, street_name):
        street_exists = False
        remove_key = ""

        for key, value in self.data.iteritems():
            if street_name.lower() == key.lower():
                street_exists = True
                remove_key = key

        if not street_exists:
            raise CustomError("Street doesn't exist")

        del self.data[remove_key]


def on_segment(x1, y1, x_mid, y_mid, x2, y2):
    if max(x1, x2) >= x_mid >= min(x1, x2) and max(y1, y2) >= y_mid >= min(y1, y2):
        return True
    return False

def orientation(x1,y1,x_mid,y_mid, x2,y2):
    val = (y_mid - y1) * (x2 - x_mid) - (x_mid-x1) * (y2 - y_mid)

    if val == 0:
        return 0
    elif val > 0:
        return 1
    else:
        return 2

def do_intersect(x1,y1,x2,y2,x3,y3,x4,y4):
    o1 = orientation(x1,y1,x2,y2,x3,y3)
    o2 = orientation(x1,y1,x2,y2,x4,y4)
    o3 = orientation(x3,y3,x4,y4,x1,y1)
    o4 = orientation(x3,y3,x4,y4,x2,y2)

    # General intersection case
    if o1 != o2 and o3 != o4:
        return True

    # Special Cases

    # L1P1, L1P2 and L2P1 are collinear and L2P1 lies on segment L1
    if o1 == 0 and on_segment(x1, y1, x3, y3, x2, y2):
        return True

    # L1P1, L1P2 and L2P2 are collinear and L2P2 lies on segment L1
    if o2 == 0 and on_segment(x1, y1, x4, y4, x2, y2):
        return True

    # L2P1, L2P2 and L1P1 are collinear and L1P1 lies on segment L2
    if o3 == 0 and on_segment(x3, y3, x1, y1, x4, y4):
        return True

    # L2P1, L2P2 and L1P2 are collinear and L1P2 lies on segment L2
    if o4 == 0 and on_segment(x3, y3, x2, y2, x4, y4):
        return True

    return False

def on_line(x1,y1,x_mid,y_mid,x2,y2):
    crossproduct = (float(y_mid) - y1) * (x2 - x1) - (x_mid - x1) * (y2 - y1)

    # compare versus epsilon for floating point values, or != 0 if using integers
    if abs(crossproduct) > 0.00001:
        return False

    dotproduct = (x_mid - x1) * (x2 - x1) + (y_mid - y1) * (y2 - y1)
    if dotproduct < 0:
        return False

    squaredlengthba = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)
    if dotproduct > squaredlengthba:
        return False

    return True


def intersect(x1,y1,x2,y2,x3,y3,x4,y4):
    overlap = False
    xnum = ((float(x1) * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4))
    xden = ((float(x1) - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4))
    if xden != 0:
        xcoor = xnum / xden
    else:
        xcoor = 0
        overlap = True

    ynum = (float(x1) * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)
    yden = (float(x1) - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)
    if yden != 0:
        ycoor = ynum / yden
    else:
        ycoor = 0
        overlap = True

    return overlap, xcoor, ycoor

def get_edges(x1,y1,x2,y2,intersection_set,vertices,edges):
    dist = []
    if (x1,y1) in vertices.values() and (x2,y2) in vertices.values():
        start_v = get_key(vertices, (x1,y1))
        end_v = get_key(vertices, (x2, y2))
    else:
        return edges



    # print(vertices)
    # print(x2,y2)



    for key, value in intersection_set.items():
        dist.append((key, ((float(x1)-value[0])**2+(float(y1)-value[1])**2)**0.5))

    sorted_dist = sorted(dist, key = lambda d: d[1])


    for i in range(len(sorted_dist)):
        if i == 0:
            if (start_v, sorted_dist[i][0]) not in edges and start_v != sorted_dist[i][0]:
                edges.append((start_v, sorted_dist[i][0]))
        else:
            if (sorted_dist[i-1][0],sorted_dist[i][0]) not in edges and sorted_dist[i-1][0] != sorted_dist[i][0]:
                edges.append((sorted_dist[i-1][0],sorted_dist[i][0]))

        if i + 1 == len(sorted_dist) and sorted_dist[i][0] != end_v :
            if (sorted_dist[i][0], end_v) not in edges and sorted_dist[i][0] != end_v:
                edges.append((sorted_dist[i][0], end_v))

    if len(intersection_set) == 0:
        edges.append((start_v,end_v))

    return edges


def get_key (x,value):
    for key, val in x.items():
        if val == value:
            return key

    print("looking for", value, "in",x)
    raise CustomError("Key not found in dictionary")




class GraphBuilder(object):
    def __init__(self, database):
        self.database = database
        self.v = {}
        self.e = []

    def build(self):
        data_set = self.database.data.items()
        intersections = False
        v_index = 0
        # Go inside list of tuples of street names and their coords
        for i in range(len(data_set)):
            # Check each line segment in your current i-th street against each line segment of the other streets
            m = 0
            # Enter loop of street 1 m-th segment
            while m + 1 < len(data_set[i][1]):
                x1 = data_set[i][1][m][0]
                y1 = data_set[i][1][m][1]
                x2 = data_set[i][1][m + 1][0]
                y2 = data_set[i][1][m + 1][1]

                intersection_set = {}
                j = 0
                # Enter j-th second street
                while j < len(data_set):

                    if j == i:
                        j += 1
                    else:
                        n = 0
                        previous_check_street_segment_overlap = False
                        # Enter loop that checks every n-th segment of street 2 against segment m of street 1
                        while n + 1 < len(data_set[j][1]):

                            if do_intersect(data_set[i][1][m][0], data_set[i][1][m][1], data_set[i][1][m+1][0],
                                            data_set[i][1][m+1][1], data_set[j][1][n][0], data_set[j][1][n][1],
                                            data_set[j][1][n+1][0], data_set[j][1][n+1][1]):

                                x3 = data_set[j][1][n][0]
                                y3 = data_set[j][1][n][1]
                                x4 = data_set[j][1][n+1][0]
                                y4 = data_set[j][1][n+1][1]

                                overlap, x_int, y_int = intersect(x1, y1, x2, y2, x3, y3, x4, y4)
                                if not overlap and not previous_check_street_segment_overlap and not on_line(x3,y3,x1,y1,x4,y4):
                                    if (x1,y1) not in self.v.values():
                                        self.v[v_index] = (x1,y1)
                                        v_index += 1
                                    if (x2,y2) not in self.v.values():
                                        self.v[v_index] = (x2,y2)
                                        v_index += 1

                                    int_index = 0
                                    if (x_int,y_int) not in self.v.values():
                                        self.v[v_index] = (x_int,y_int)
                                        int_index = v_index
                                        v_index += 1
                                    else:
                                        int_index = get_key(self.v,(x_int,y_int))

                                    intersection_set[int_index] = (x_int, y_int)

                                    if (x3,y3) not in self.v.values():
                                        self.v[v_index] = (x3,y3)
                                        v_index += 1

                                    if (x4, y4) not in self.v.values():
                                        self.v[v_index] = (x4,y4)
                                        v_index += 1

                                elif not overlap:
                                    previous_check_street_segment_overlap = False

                                else:
                                    previous_check_street_segment_overlap = True

                                    if (x2,y2) == (x3,y3):
                                        if (x1,y1) not in self.v.values():
                                            self.v[v_index] = (x1, y1)
                                            v_index += 1
                                        int_index = 0
                                        if (x2, y2) not in self.v.values():
                                            self.v[v_index] = (x2, y2)
                                            int_index = v_index
                                            v_index += 1
                                        else:
                                            int_index = get_key(self.v, (x2, y2))

                                        intersection_set[int_index] = (x2, y2)
                                    if on_line(x1,y1,x3,y3,x2,y2):

                                        if (x1, y1) not in self.v.values():
                                            self.v[v_index] = (x1, y1)
                                            v_index += 1

                                        if (x2, y2) not in self.v.values():
                                            self.v[v_index] = (x2, y2)
                                            v_index += 1

                                        int_index = 0
                                        if (x3, y3) not in self.v.values():
                                            self.v[v_index] = (x3, y3)
                                            int_index = v_index
                                            v_index += 1
                                        else:
                                            int_index = get_key(self.v, (x3, y3))



                                        intersection_set[int_index] = (x3, y3)

                                    if on_line(x3,y3,x1,y1,x4,y4):
                                        int_index = 0
                                        if (x1, y1) not in self.v.values():
                                            self.v[v_index] = (x1, y1)
                                            int_index = v_index
                                            v_index += 1
                                        else:
                                            int_index = get_key(self.v, (x1, y1))
                                        intersection_set[int_index] = (x1, y1)

                                        if (x2, y2) not in self.v.values():
                                            self.v[v_index] = (x2, y2)
                                            v_index += 1

                                        if (x3, y3) not in self.v.values():
                                            self.v[v_index] = (x3, y3)
                                            v_index += 1

                                    if (x3,y3) in intersection_set.values():
                                        if (x4, y4) not in self.v.values():
                                            self.v[v_index] = (x4, y4)
                                            v_index += 1

                                intersections = True

                            n += 1

                        j += 1

                if intersections:
                    self.e = get_edges(x1, y1, x2, y2, intersection_set, self.v, self.e)
                    intersections = False
                m += 1

        return self.v, self.e


class PrintGraph(object):
    def __init__(self, v, e):
        self.v = v
        self.e = e

    def print_graph(self):
        output = "V " + str(len(self.v))
        output += "\n"
        output += "E {"
        for value in self.e:
            output += "<" + str(value[0]) + "," + str(value[1]) + ">,"
        if output[-1] == ",":
            output = output[:-1]
        output += "}\n"
        sys.stdout.write(output)


def main():
    # YOUR MAIN CODE GOES HERE

    # sample code to read from stdin.
    # make sure to remove all spurious print statements as required
    # by the assignment

    street_db = StreetDB()

    while True:
        line = sys.stdin.readline()
        if line == "":
            break

        try:
            if line[-1] == '\n':
                line = line[:-1]
            user_input = parse_input(line)

            if user_input[0] == 'a':
                street = user_input[1]
                pts = user_input[2]
                street_db.add(street, pts)
            elif user_input[0] == 'c':
                street = user_input[1]
                pts = user_input[2]
                street_db.change(street, pts)
            elif user_input[0] == 'r':
                street = user_input[1]
                street_db.remove(street)
            elif user_input[0] == 'g':
                graph = GraphBuilder(street_db)
                graph.build()
                graph_printer = PrintGraph(graph.v,graph.e)
                graph_printer.print_graph()
                sys.stdout.flush()
            else:
                raise CustomError("Invalid command")

        except Exception as err:
            sys.stderr.write("Error:" + " " + str(err) + "\n")
            sys.stdout.flush()



    # return exit code 0 on successful termination
    sys.exit(0)


if __name__ == '__main__':
    main()
