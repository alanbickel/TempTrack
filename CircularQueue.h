#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#include <Arduino.h>
#include <SystemConstants.h>


//Circularly linked list template, inserts at end of queue, sungle-pointer implementation
// utilized to reduce node size, front of que accessed by last->nextNode;
template <class T>
class CircularQueue{

private:

	struct ListNode
	{
		T info;

		ListNode * nextNode;
	};


	int length;
	int maxItems;
	ListNode * last;					//point to end of list, last->next points to front of list

	//identical function to deque(), used internally to empty the que, supplies to external function call
	void removeNode() {

		if (!isEmpty()) {
			if (last == last->nextNode) {			// only one element in list
				ListNode *temp = NULL;
				temp = last;		// hold value pointer to by *last
				delete temp;		//free memory
				last = NULL;		//clear last pointer for re-use
				length = 0;								// no elements in list
			}
			else {
				ListNode *temp = NULL;
				temp = last->nextNode;
				last->nextNode = temp->nextNode;		// step up front of list
				delete temp;							// clear 'old' front element
				length--;								// decrement length
			}
		}
	}

public:
	//CTOR
	CircularQueue(int max){
		maxItems = max;
		last = NULL;
		length = 0;
	}

	//DTOR, free memory
	~CircularQueue(){
		makeEmpty();
	}

	//INSERT NEW VALUE TO END OF LIST : THROWS std::bad_alloc EXCEPTION
	void enque(T value){
		
		if (!isFull()){

			// new container for data
			ListNode *newNode = NULL;
			newNode = new  struct ListNode;

			// set struct values
			newNode->nextNode = NULL;
			newNode->info = value;

			if (!last){					// if empty list ,insert initial value
				last = newNode;
				newNode->nextNode = last;
			}

			else {						// add to back of list

				newNode->nextNode = last->nextNode;		// new last point to front of list
				last->nextNode = newNode;				// previous last now points to new last
				last = newNode;							// new value at end of list

			}

			length++;											//increment number of elements


		}
	}

	//REMOVE PREMIER ELEMENT FROM LIST, PASS TO REFERENCE VAR : THROWS QUEUE_EXCEPTION
	void dequeue(T &target){
		if (!isEmpty()){



			if (last == last->nextNode){			// only one element in list

				target = last->info;				//return value
				ListNode *temp = NULL;				
				temp = last;		// hold value pointer to by *last
				delete temp;		//free memory

				last = NULL;		//clear last pointer for re-use

				length = 0;								// no elements in list

			}
			else {
				ListNode *temp = NULL;
				temp = last->nextNode;
				target = temp->info;					// provide data to caller

				last->nextNode = temp->nextNode;		// step up front of list

				delete temp;							// clear 'old' front element
				length--;								// decrement length
			}
		}
	}

	//RETURN COPY OF PREMIER ELEMENT IN LIST  
	void top(T &target){
		if (!isEmpty()){
			ListNode *front = NULL;
			front = last->nextNode;

			if (!front){								// only one element in list
				target = last->info;
			}
			else {
				target = front->info;					// provide data to caller
			}
		}

	}

	void bottom(T &target){
		if (!isEmpty()){
			target = last->info;
		}

	}

	//EVALUATE IF ANY CONTENTS
	bool isEmpty(){
		return(length == 0);
	}

	//GET NUMBER ITEMS IN LIST
	int getLength(){
		return length;
	}

	bool isFull(){
		return (length == maxItems);
	}

	//CLEAR LIST OF ALL DATA
	void makeEmpty() {
		while (!isEmpty()) {
			removeNode();
		}
		
	}

	/*scan queue for item*/
	void find(T item, bool &found){

		found = false;

		ListNode * begin = NULL;
		ListNode * terminate = NULL;		// remember beginning search point to kill search after one full iteration

		if (! isEmpty()){

			begin = last;
			terminate = last;

			while (!found){

				if (begin->info == item){
					found = true;
				}
				else {
					begin = begin->nextNode;
					

					if (begin == terminate) {
						break;
					}
				}
			}
		}
	}
};
#endif