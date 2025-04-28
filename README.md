# Densest Subgraph Discovery - Assignment 2

**Starting Webpage:**  
👉 [Click Here to View the Project](https://krish080403.github.io/DAA-Assignment-2/Hosting/main.html)

---

## About the Project

This project implements algorithms for the **Densest Subgraph Discovery (DSD)** problem from the research paper:  
**Efficient Algorithms for Densest Subgraph Discovery** by Yixiang Fang et al. (VLDB 2019).  
The main goal of DSD is to find a subgraph of a given graph that has the maximum density according to specific metrics.

In this assignment, we specifically:
- Implemented **Algorithm 1 (Exact Algorithm)** and **Algorithm 4 (CoreExact Algorithm)** from the paper.
- Focused on the discovery of densest subgraphs based on **h-clique density**.
- Took the value of **h = 3**, meaning that we consider **triangles** (3-cliques) for density calculations.

---

## Algorithms Implemented

We implemented two main algorithms from the paper:

### 1. Algorithm 1: Exact Algorithm (From Section 4.1)

**Objective:**  
Find the exact densest subgraph with respect to h-clique density (triangle density in our case, h=3).

**Method:**  
- Constructs a **flow network** based on the input graph and performs **binary search** over possible density values.
- For each guess of density (`α`), a minimum **s-t cut** is computed to check if there exists a subgraph with density at least `α`.
- Continues binary search until the densest subgraph is found.

**Key Steps:**
- Enumerate all (h-1)-cliques (edges for triangles).
- Build a flow network connecting vertices, (h-1)-cliques, and source/sink nodes.
- Perform minimum cut to guide binary search.
- Stop when the guess range is narrow enough.

**Time Complexity:**  
\( O\left( n \times \binom{d-1}{h-1} + (n \times |\Lambda| + \min(n, |\Lambda|)^3) \times \log n \right) \)

where:
- \( n \) = number of vertices,
- \( d \) = maximum degree,
- \( \Lambda \) = set of (h-1)-cliques.

---

### 2. Algorithm 4: CoreExact Algorithm (From Section 6.1)

**Objective:**  
Improve the efficiency of the Exact Algorithm by restricting the search to **cores**.

**Method:**  
- First computes the **(k,Ψ)-core** where Ψ is an h-clique (triangle here).
- Uses the **core decomposition** to identify subgraphs with high triangle participation.
- Prunes the graph to a much smaller subgraph and applies the binary search only on these small cores.
- This significantly speeds up the computation compared to using the entire graph.

**Optimization Techniques:**
- Tighter initial bounds for binary search based on core numbers.
- Locate the candidate subgraph inside the (k,Ψ)-core.
- Gradually narrow down the working graph as binary search progresses.

**Time Complexity:**  
Much faster than Algorithm 1 in practice — core decomposition is \( O\left( n \times \binom{d-1}{h-1} \right) \) and the reduced size of graphs makes the binary search much cheaper.

---

## Value of **h**

In this project, we set the value of **h = 3**, meaning:
- We are finding the **densest subgraph based on triangle participation**.
- A triangle (3-clique) is considered a basic building block of density rather than just edges.

The algorithm computes the number of triangles each node participates in and uses that for core decomposition and densest subgraph discovery.

---

## How to Run the Code

The code is hosted as a webpage at:

🔗 [DAA Assignment 2 Webpage](https://krish080403.github.io/DAA-Assignment-2/Hosting/main.html)

**Instructions:**
1. Download the datasets on which you want to run the algorithm
2. Choose which algorithm you want to run:
   - **Exact Algorithm** for precise but slower computation.
   - **CoreExact Algorithm** for faster results using cores.
3.g++ <algorithm.cpp> -o output    |    ./ouput <dataset_name>   
---

## References

- **Main Paper:**  
Yixiang Fang, Kaiqiang Yu, Reynold Cheng, Laks V.S. Lakshmanan, Xuemin Lin.  
*"Efficient Algorithms for Densest Subgraph Discovery."*  
PVLDB 12(11): 1719-1732, 2019. [DOI Link](https://doi.org/10.14778/3342263.3342645)

