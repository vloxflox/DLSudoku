#include "node.h"
#include "linkedMesh.h"
#include <iostream>
#include <vector>

// implementado com base em https://bit.ly/2oHRcon

//81x4 para sudokus padrão
#define SUDOKUWIDTH 324

//retornam a prox. linha/coluna lidando com a caracteristica circular
//lida também com a coluna extra dos headers

//col à direita
int LinkedMesh::getRight(int i){return (i+1) % nCol;}
//col à esquerda
int LinkedMesh::getLeft(int i){
	if(i-1 < 0){
		return nCol-1;
	}else{
		return i-1;
	}
}
//linha acima
int LinkedMesh::getUp(int i){
	if(i-1 < 0){
		//não desconta 1 pois existe uma linha a mais para headers
		return nRow;
	}else{
		return i-1;
	}
}   
//linha abaixo
int LinkedMesh::getDown(int i){return (i+1) % (nRow+1);} 

LinkedMesh::LinkedMesh(){}

LinkedMesh::LinkedMesh(bool ProbMat[100][50]){

	header = new Node();

	initialize(ProbMat);
}

void LinkedMesh::initialize(bool ProbMat[100][50]){
	// One extra row for list header nodes 
    // for each column

	std::cout<<"Inicializando nodes"<<std::endl;

    nRow = 7;
	nCol = 7;

	std::cout<<"num de colunas :"<<nRow<<std::endl;
	std::cout<<"num de linhas  :"<<nCol<<std::endl;


    for(int i = 0; i <= nRow; i++) 
    { 
    	std::cout<<"i :" << i <<std::endl<<std::endl;
        for(int j = 0; j < nCol; j++) 
        { 
        	std::cout<<"j :" << j <<std::endl;
            //"há um 1 nessa etapa da matriz" 
            if(ProbMat[i][j]) 
            { 
                int a, b; 
  			
                // Atualizar a contagem de elementos na coluna do header de coluna
                if(i!=0){
                	Matrix[0][j].nodeCount += 1; 
                }
  
                // Associa o header de coluna ao campo de coluna
                Matrix[i][j].column = &Matrix[0][j]; 
  
                // coloca id de linha e coluna 
                Matrix[i][j].rowID = i; 
                Matrix[i][j].colID = j; 
  
                // Liga com os '1's vizinhos (considerando a "circularidade" da lista)
  

                // Left pointer 
                a = i; b = j; 
                do{ b = getLeft(b); } while(!ProbMat[a][b] && b != j); 
                Matrix[i][j].left = &Matrix[i][b]; 

                // Right pointer 
                a = i; b = j; 
                do { b = getRight(b); } while(!ProbMat[a][b] && b != j); 
                Matrix[i][j].right = &Matrix[i][b]; 
  

                // Up pointer 
                a = i; b = j; 
                do { a = getUp(a); } while(!ProbMat[a][b] && a != i); 
                Matrix[i][j].up = &Matrix[a][j]; 
  

                // Down pointer 
                a = i; b = j; 
                do { a = getDown(a); } while(!ProbMat[a][b] && a != i); 
                Matrix[i][j].down = &Matrix[a][j]; 
            }
        }

    }

    std::cout<<"Nodes inicializados"<<std::endl;

    std::cout<<"Marcando header"<<std::endl;

    // link header right pointer to column  
    // header of first column  
    header->right = &Matrix[0][0]; 
  
    // link header left pointer to column  
    // header of last column  
    header->left = &Matrix[0][nCol-1]; 
  
    Matrix[0][0].left = header; 
    Matrix[0][nCol-1].right = header;

    std::cout<<"Header Marcado"<<std::endl;
}

//Tira um node da lista
void LinkedMesh::cover(Node *targetNode) 
{ 
    Node *row, *rightNode; 
   
   	//header da coluna do nó 
    Node *colNode = targetNode->column; 
  
    // desliga a coluna da lista dos indexados
    colNode->left->right = colNode->right; 
    colNode->right->left = colNode->left; 
  
    // Move para baixo e remove todos os nodes que tiverem conexão (indo para a direita)
    for(row = colNode->down; row != colNode; row = row->down) 
    { 
        for(rightNode = row->right; rightNode != row; 
            rightNode = rightNode->right) 
        { 
            rightNode->up->down = rightNode->down; 
            rightNode->down->up = rightNode->up; 
  
            // atualiza nodecount do header
            Matrix[0][rightNode->colID].nodeCount -= 1; 
        } 
    } 
}

//repõe um node na lista
void LinkedMesh::uncover(Node *targetNode) 
{ 
    Node *rowNode, *leftNode; 
  
    Node *colNode = targetNode->column; 
  
    // Liga de volta movendo para baixo e para a direita
    for(rowNode = colNode->up; rowNode != colNode; rowNode = rowNode->up) 
    { 
        for(leftNode = rowNode->left; leftNode != rowNode; 
            leftNode = leftNode->left) 
        { 
            leftNode->up->down = leftNode; 
            leftNode->down->up = leftNode; 
  
            // Atualiza contagem de nodes 
            Matrix[0][leftNode->colID].nodeCount += 1; 
        } 
    } 
  
    // Liga de volta com os vizinhos
    colNode->left->right = colNode; 
    colNode->right->left = colNode; 
}

//Retorna a coluna com menos nodes
//Acelera o processo de decisão
Node* LinkedMesh::getMinColumn() 
{ 
    Node *h = header; 
    Node *min_col = h->right; 
    h = h->right->right; 
    do
    { 
        if(h->nodeCount < min_col->nodeCount) 
        { 
            min_col = h; 
        } 
        h = h->right; 
    }while(h != header); 
  
    return min_col; 
}

void LinkedMesh::printSolutions() 
{ 
    std::cout<<"Printing Solutions: "; 
    std::vector<struct Node*>::iterator i; 
  
    for(i = solutions.begin(); i!=solutions.end(); i++) 
        std::cout<<(*i)->rowID<<" "; 
    std::cout<<"\n"; 
}
  
// Search for exact covers 
void LinkedMesh::search(int k) 
{
	std::cout<<"test11111"<<std::endl;
    Node *rowNode; 
    Node *rightNode; 
    Node *leftNode; 
    Node *column; 
  
    std::cout<<"entrando no search"<<std::endl;
    // Header list vazia = Resolvido
    if(header->right == header) 
    { 
        printSolutions(); 
        return; 
    } 
  
    // Escolhe a coluna
    column = getMinColumn(); 
    std::cout<<"escolhendo coluna :"<< column->colID<<std::endl;
  
    // cobre a coluna
    cover(column); 
  
    for(rowNode = column->down; rowNode != column;  
        rowNode = rowNode->down ) 
    { 
        solutions.push_back(rowNode); 
  
        for(rightNode = rowNode->right; rightNode != rowNode; 
            rightNode = rightNode->right) 
            cover(rightNode); 
  
        // move to level k+1 (recursively) 
        search(k+1); 
  
        // if solution in not possible, backtrack (uncover) 
        // and remove the selected row (set) from solution 
        solutions.pop_back(); 
  
        column = rowNode->column; 
        for(leftNode = rowNode->left; leftNode != rowNode; 
            leftNode = leftNode->left) 
            uncover(leftNode); 
    } 
  
    uncover(column); 
}