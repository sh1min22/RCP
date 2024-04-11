# Connectivity-Oriented Property Graph Partitioning for Distributed Graph Pattern Query Processing

# Overview
relationship connectivity partitioning(RCP) is a edge-centric property graph partitioning approach, where the objective function is to minimize the cuts to the number of distinct crossing properties. This approach can be used to avoid inter-partition joins in a wider set of Cypher/Gremlin workloads in the context of distributed Cypher/Gremlin query evaluation.

# Preprocessing
This section outlines the steps and methodologies involved in preprocessing the data.

## csv2txt.cpp
This file is used to divide the edge csv files in the original data set into the corresponding txt files according to the relationship.

# Partitioning
The partitioning section describes how the dataset is divided or segmented into different parts. Partitioning can be based on various criteria, such as data type, usage, or other characteristics relevant to the project. This process is crucial for managing and analyzing data more efficiently.

# Benchmark Queries
The benchmark queries used in our experimental evaluation exists in #queries# folder.

If you encounter any problems, please send emails to me (email address: shimin22@hnu.edu.cn).
