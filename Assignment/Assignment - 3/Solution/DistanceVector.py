#//==================================
#// Computer Networks & Applications
#// Student: Vandit Jyotindra Gajjar
#// Student ID: a1779153
#// Semester: 1
#// Year: 2020
#// Assignment: 3
#//===================================

#Importing utilities copy and sys
import copy
import sys

#Defining function min_column accepts two parameters: table and column_no
#This function returns best cost path. 
def min_column(table, column_no):
	value_list = []
	for i in range(0,len(table)):
		if table[i][column_no] != None:
			value_list.append(table[i][column_no])
	if(len(value_list) == 0):
		return None
	else:
		return min(value_list)

#Defining route_through function accepts four parameters: value, table, column_no, and node_no
#This function adds value to the routing table.
def route_through(value, table, column_no, node_no):
	for i in range(0, len(table)):
		if(node_no != i):
			if table[i][column_no] == value:
				return i

#Defining initialize_tables function accepts four parameters: nodes, routing_table_node, links, and count
#This function initialize the table based on nodes and edge costs. 
def initialize_tables(nodes, routing_table_node, links, count):
	for i in range(0,len(routing_table_node)):
		for j in links:
			j = j.split()
			if(i == nodes[j[0]]):
				routing_table_node[i][i][nodes[j[1]]] = int(j[2])
				print("t="+str(count)+" distance from "+ nodes[i] +" to "+j[1]+" via "+j[1]+" is "+str(j[2]))
			if(i == nodes[j[1]]):
				routing_table_node[i][i][nodes[j[0]]] = int(j[2])
				print("t="+str(count)+" distance from "+ nodes[i] +" to "+j[0]+" via "+j[0]+" is "+str(j[2]))
			nodes[str(j[0])+str(j[1])]= int(j[2])
			nodes[str(j[1])+str(j[0])]= int(j[2])
		routing_table_node[i][i][i] = 0
	count += 1
	return routing_table_node, count, nodes

#Defining update_table accepts three parameters: nodes, routing_table_node, and count
#This function update the routing table till convergence is achieved based on the algorithm. 
def update_table(nodes, routing_table_node, count):
	print(" ")
	change = 0
	old_table = copy.deepcopy(routing_table_node)
	for i in range(0, len(routing_table_node)):
		for j in range(0, len(routing_table_node[i])):
			if(routing_table_node[i][i][j] != None): #check neighbour
				for k in range(0, len(routing_table_node[i][j])):
					if(min_column(routing_table_node[j],j) != None and min_column(old_table[j],k) != None):
						if(k != i and i!=j
							and routing_table_node[i][j][k] !=
							 min_column(old_table[j],k) + nodes[nodes[i]+nodes[j]]):
							routing_table_node[i][j][k] = min_column(old_table[j],k) + nodes[nodes[i]+nodes[j]]#min_column(old_table[i],j)							
							if(min_column(old_table[i],j) != min_column(old_table[j],k) + nodes[nodes[i]+nodes[j]] and j != k):
								print("t="+str(count)+" distance from "+ nodes[i] +" to "+nodes[k]+" via "+nodes[j]+" is "+str(routing_table_node[i][j][k]))
							change = 1
						if(k == i):
							routing_table_node[i][j][k] = 0
	return change, routing_table_node

#Defining print_routing_routes accepts two parameters: nodes and routing_table_node
#This function prints best path for each router based on the routing table.
def print_routing_routes(nodes, routing_table_node):
	for i in range(0, len(routing_table_node)):
		for j in range(0, len(routing_table_node[i])):
			if(i!=j):
				print("router "+nodes[i]+": "+nodes[j]+" is "+str(min_column(routing_table_node[i], j))+" routing through "+ nodes[route_through(min_column(routing_table_node[i],j), routing_table_node[i], j, i)])
		print(" ")

#Defining reinitialize_tables accepts four parameters: nodes, routing_table_node, links, and count
#This function reinitialize the routing table based on changed link weights. 
def reinitialize_tables(nodes, routing_table_node, links, count):
	for i in range(0,len(routing_table_node)):
		for j in links:
			j = j.split()
			if(len(j) != 3):
				continue
			if(i == nodes[j[0]]):
				routing_table_node[i][i][nodes[j[1]]] = int(j[2])
				print("t="+str(count)+" distance from "+ nodes[i] +" to "+j[1]+" via "+j[1]+" is "+str(j[2]))
			if(i == nodes[j[1]]):
				routing_table_node[i][i][nodes[j[0]]] = int(j[2])
				print("t="+str(count)+" distance from "+ nodes[i] +" to "+j[0]+" via "+j[0]+" is "+str(j[2]))
			nodes[str(j[0])+str(j[1])]= int(j[2])
			nodes[str(j[1])+str(j[0])]= int(j[2])
	count += 1
	return routing_table_node, count, nodes

#Defining changed_configuration accepts three parameters: nodes, routing_table_node, and changed_config
#This function accepts changeConfigL file and update the routing table based on the changed link weight.
def changed_configuration(nodes, routing_table_node, changed_config):
	count = 0
	routing_table_node, count, nodes = reinitialize_tables(nodes, routing_table_node, changed_config, count)

	change = 1
	while(change == 1):
		change, routing_table_node = update_table(nodes, routing_table_node, count)
		count += 1
	return routing_table_node

#This part of the code reads the configL file. 
configL = open(str(sys.argv[1]), "r")
configuration = configL.read().split('\n')
configL.close()

#This part of the code reads the changeConfigL file. 
changeConfigL = open(str(sys.argv[2]), "r")
changed_config = changeConfigL.read().split('\n')
changeConfigL.close()

next_line_no=0
no_of_nodes = int(configuration[next_line_no])
next_line_no += 1

nodes = {}
for i in range(0, no_of_nodes):
	nodes[configuration[next_line_no+i]] = i
	nodes[i] = configuration[next_line_no+i]
print("\n#START\n")

next_line_no += no_of_nodes
no_of_links = int(configuration[next_line_no])
next_line_no += 1

routing_table_node = []
for i in range(0,no_of_nodes):
	routing_table_node.append([])
	for j in range(0,no_of_nodes):
		routing_table_node[i].append([])
		for k in range(0,no_of_nodes):
			routing_table_node[i][j].append(None)

links = []
for i in range(0, no_of_links):
	links.append(configuration[next_line_no+i])

count = 0
routing_table_node, count, nodes = initialize_tables(nodes, routing_table_node, links, count)

change = 1
while(change == 1):
	change, routing_table_node = update_table(nodes, routing_table_node, count)
	count += 1

print("\n#INITIAL \n")
print_routing_routes(nodes, routing_table_node)

print("\n#UPDATE\n")
routing_table_node = changed_configuration(nodes, routing_table_node, changed_config[1:len(changed_config)])

print("\n#FINAL\n")
print_routing_routes(nodes, routing_table_node)