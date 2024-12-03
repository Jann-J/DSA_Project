#define TNX_SIZE 32
#define ID_SIZE 20

/* 
* Parameters: pointers to two `Info` structs 
* 	      (`Info **a`, `Info **b`) 
* Function: Swaps the values of the two pointers
* to effectively swap the structs they point to
* Return value: None
*/
void swap(Info **a, Info **b){
    Info *temp = *a;
    *a = *b;
    *b = temp;
}

/*
* Parameters: `Info **array`: An array of pointers to `Info` structs
* 	      `int n`: The size of the heap (or array)
*	      `int i`: The index of the current node being heapified
* Function: Ensures the max-heap property for the node at index `i`
* by comparing it with its left and right children. If a child is larger, 
* the node swaps places with the larger child, and the function is recursively called on the child.
* Return value: None
*/
void heapify(Info **array, int n, int i) {
	int largest, left, right;
	largest = i;
    left = 2 * i + 1;
    right = 2 * i + 2;

    // Compare left child with root
    if (left < n && strcmp(array[left]->senderID, array[largest]->senderID) > 0) {
        largest = left;
    }

    // Compare right child with the largest so far
    if (right < n && strcmp(array[right]->senderID, array[largest]->senderID) > 0) {
        largest = right;
    }

    // Swap and continue heapifying if root is not the largest
    if (largest != i) {
        swap(&array[i], &array[largest]);
        heapify(array, n, largest);
    }
    return;
}

/* 
* Parameters: `Info **array`: An array of pointers to `Info` structs
*	      `int n`: The size of the array
* Function: Sorts the array in ascending order of `senderID` using heapsort. 
* First, the heap is built by heapifying all nodes. Then, the largest element (root of the heap) 
* is swapped with the last element, and the heap size is reduced for the next iteration.
* Return value: None
*/
void heapSort(Info **array, int n) {
    // Build the heap
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(array, n, i);
    }

    // Extract elements from heap one by one
    for (int i = n - 1; i > 0; i--) {
        swap(&array[0], &array[i]);
        heapify(array, i, 0);
    }
}

/* Parameters: `Info **transactions`: An array of pointers to `Info` structs
* 	       `int n`: The size of the array
* Function: A wrapper function for heapsort that sorts transactions based on `senderID`.
* Return value: None
*/
void sortTransactions(Info **transactions, int n) {
    heapSort(transactions, n);
}
