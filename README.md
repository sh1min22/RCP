# Connectivity-Oriented Property Graph Partitioning for Distributed Graph Pattern Query Processing

# Overview
relationship connectivity partitioning(RCP) is a edge-centric property graph partitioning approach, where the objective function is to minimize the cuts to the number of distinct crossing properties. This approach can be used to avoid inter-partition joins in a wider set of Cypher/Gremlin workloads in the context of distributed Cypher/Gremlin query evaluation.

# Preprocessing
This section outlines the steps and methodologies involved in preprocessing the data.

## csv2txt.cpp
This file is used to divide the edge csv files in the original data set into the corresponding txt files according to the relationship.

## graph_v4.cpp
This file is used to process the txt files obtained in the previous step into the corresponding LCC set. The set of interconnected nodes is divided into an LCC by connecting different nodes by edges.

# Partitioning
This section provides an overview of the steps and methods involved in partitioning.

## partition_v4.cpp
This file is used to divide the LCC obtained in the previous step into different partitions considering both cost and benefit.

## distributeData_nodes.cpp
This file is used to divide the total node table into sub-node tables of each partition based on the set of nodes belonging to each partition obtained in the previous step.

## distributeData_relationships.cpp
This file is used to partition the total edge table into sub-edge tables of each partition based on the set of LCCS belonging to each partition obtained in the previous step.

## txt2csv.cpp

# Benchmark Queries
The benchmark queries used in our experimental evaluation exists in #queries# folder.

If you encounter any problems, please send emails to me (email address: shimin22@hnu.edu.cn).
