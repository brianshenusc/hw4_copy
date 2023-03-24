#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* current);
    void rotateRight(AVLNode<Key, Value>* parent);
    void rotateLeft(AVLNode<Key, Value>* parent);
    void removeFix(AVLNode<Key,Value>* parent, int diff);

};



/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO

    AVLNode<Key, Value>* duplicates = static_cast<AVLNode<Key, Value>*>(this->internalFind(new_item.first));
	// if there is a duplicate, replace the value
	if(duplicates != NULL) {
        // std::cout << "HAS DUPE" << std::endl;
		duplicates->setValue(new_item.second);
	}
	// if root is NULL, then insert a new node with new_item's variables to root
	else if(this->root_ == NULL) {
		AVLNode<Key, Value>* temp = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
		this->root_ = temp; 
        return;
	}
	// otherwise, create a new Node with new_item's variables and insert into the BST
	else {
		AVLNode<Key, Value>* temp = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
		AVLNode<Key, Value>* parent = static_cast<AVLNode<Key, Value>*>(this->root_);
		bool isDone = false;
		while(!isDone) {
			// if the current key value is less than the root's key value
			if(temp->getKey() < parent->getKey()) {
				// if parent node's left child is NULL, then sets it to the new value
				if(parent->getLeft() == NULL) {
					temp->setParent(parent);
					parent->setLeft(temp);  
					isDone = true; 
				}
				// if the parent's left child is not NULL, then continues down the BST
				else {
					parent = parent->getLeft();
				}
			}
			// if the current key value is greater than  the root's key value
			else if(temp->getKey() > parent->getKey()) {
				// if parent node's right child is NULL, then sets it to the new value
				if(parent->getRight() == NULL) {
					temp->setParent(parent);
					parent->setRight(temp);  
					isDone = true; 
				}
				// if the parent's right child is not NULL, then continues down the BST
				else {
					parent = parent->getRight();
				}
			}
		}
        // sets the balance
        if(parent->getBalance() == 1 || parent->getBalance() == -1) {
            parent->setBalance(0);
            return;
        }
        else {
            if(parent->getLeft() != NULL && parent->getRight() == NULL) {
                parent->setBalance(-1);
            }
            else if(parent->getRight() != NULL && parent->getLeft() == NULL) {
                parent->setBalance(1);
            }
            insertFix(parent, temp);
        }
	}
}


template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* current) {
    if(parent->getParent() == NULL || parent == NULL) {
        return;
    }
    // if the child is a left child of the parent's parent
    if(parent->getParent()->getLeft() == parent) {
        parent->getParent()->updateBalance(-1);
        // case 1
        if(parent->getParent()->getBalance() == 0) {
            // std::cout << "case 1" << std::endl;
            return;
        }
        // case 2
        else if(parent->getParent()->getBalance() == -1) {
            // std::cout << "case 2" << std::endl;
            insertFix(parent->getParent(), parent);
            return;
        }
        else if(parent->getLeft() == current || parent->getRight() == current) {
            // std::cout << "case 3" << std::endl;
            parent->setBalance(0);
            parent->getParent()->setBalance(0);
            rotateRight(parent);
        }
    }
    else if(parent->getParent()->getRight() == parent) {
        parent->getParent()->updateBalance(1);
        // case 1
        if(parent->getParent()->getBalance() == 0) {
            // std::cout << "case 1" << std::endl;
            return;
        }
        // case 2
        else if(parent->getParent()->getBalance() == 1) {
            // std::cout << "case 2" << std::endl;
            insertFix(parent->getParent(), parent);
            return;
        }
        else if(parent->getRight() == current || parent->getLeft() == current) {
            // std::cout << "case 3" << std::endl;
            parent->setBalance(0);
            parent->getParent()->setBalance(0);
            rotateLeft(parent);
        }
    }
}



template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* parent) {
    // std::cout << "1" << std::endl;
    AVLNode<Key, Value>* grandparent = parent->getParent();
    AVLNode<Key, Value>* greatgrandparent = grandparent->getParent();
    // two left nodes
    if(parent->getLeft() != NULL) {
        // std::cout << "2" << std::endl;
        if(grandparent == this->root_) {
            // std::cout << "3" << std::endl;
            AVLNode<Key, Value>* child = parent->getLeft();
            this->root_ = parent;
            parent->setLeft(child);
            child->setParent(parent);
            parent->setParent(NULL);
            parent->setRight(grandparent);
            grandparent->setParent(parent);
            grandparent->setLeft(NULL);
        }
        else {
            // std::cout << "1" << std::endl;

            AVLNode<Key, Value>* child = parent->getLeft();
            // if(greatgrandparent->getRight() == grandparent) {
            //     greatgrandparent->setRight(parent);
            // }
            // else {
            //     greatgrandparent->setLeft(parent);
            // }
            // this->root_ = parent;
            parent->setLeft(child);
            child->setParent(parent);
            parent->setParent(greatgrandparent);
            parent->setRight(grandparent);
            grandparent->setParent(parent);
            grandparent->setLeft(NULL);
        }
        
    }
    // root - left - right
    else if(parent->getRight() != NULL) {
        // std::cout << "other one" << std::endl;
        if(grandparent == this->root_) {
            // std::cout << "5" << std::endl;
            AVLNode<Key, Value>* child = parent->getRight();
            this->root_ = child;
            this->root_->setLeft(parent);
            parent->setParent(this->root_);
            this->root_->setRight(grandparent);
            grandparent->setParent(this->root_);
            grandparent->setLeft(NULL);
            parent->setRight(NULL);
        }
        else {
            // std::cout << "2" << std::endl;
            
            AVLNode<Key, Value>* child = parent->getRight();
            // if(greatgrandparent->getRight() == grandparent) {
            //     greatgrandparent->setRight(child);
            // }
            // else {
            //     greatgrandparent->setLeft(child);
            // }
            // this->root_ = child;
            child->setLeft(parent);
            parent->setParent(child);
            child->setRight(grandparent);
            child->setParent(greatgrandparent);
            grandparent->setParent(child);
            grandparent->setLeft(NULL);
            parent->setRight(NULL);

        }
    }
}




template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* parent) {
    // std::cout << "1" << std::endl;
    AVLNode<Key, Value>* grandparent = parent->getParent();
    AVLNode<Key, Value>* greatgrandparent = grandparent->getParent();
    // two right nodes
    if(parent->getRight() != NULL) {
        // std::cout << "2" << std::endl;
        if(grandparent == this->root_) {
            // std::cout << "3" << std::endl;
            AVLNode<Key, Value>* child = parent->getRight();
            this->root_ = parent;
            parent->setRight(child);
            child->setParent(parent);
            parent->setParent(NULL);
            parent->setLeft(grandparent);
            grandparent->setParent(parent);
            grandparent->setRight(NULL);
        }
        else {
            // std::cout << "3" << std::endl;

            AVLNode<Key, Value>* child = parent->getRight();
            // if(greatgrandparent->getRight() == grandparent) {
            //     greatgrandparent->setRight(parent);
            // }
            // else {
            //     greatgrandparent->setLeft(parent);
            // }
            // this->root_ = parent;
            parent->setRight(child);
            child->setParent(parent);
            parent->setParent(greatgrandparent);
            parent->setLeft(grandparent);
            grandparent->setParent(parent);
            grandparent->setRight(NULL);
        }
        
    }
    // root - right - left
    else if(parent->getLeft() != NULL) {
        // std::cout << "other one" << std::endl;
        if(grandparent == this->root_) {
            std::cout << "5" << std::endl;
            AVLNode<Key, Value>* child = parent->getLeft();
            this->root_ = child;
            this->root_->setRight(parent);
            parent->setParent(this->root_);
            this->root_->setLeft(grandparent);
            grandparent->setParent(this->root_);
            grandparent->setRight(NULL);
            parent->setLeft(NULL);
        }
        else {
            // std::cout << "4" << std::endl;

            AVLNode<Key, Value>* child = parent->getLeft();
            // if(greatgrandparent->getRight() == grandparent) {
            //     greatgrandparent->setRight(child);
            // }
            // else {
            //     greatgrandparent->setLeft(child);
            // }
            // this->root_ = child;
            child->setRight(parent);
            parent->setParent(child);
            child->setLeft(grandparent);
            child->setParent(greatgrandparent);
            grandparent->setParent(child);
            grandparent->setRight(NULL);
            parent->setLeft(NULL);
        }
    }
}





/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
	AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
		
	// if there are no nodes, do nothing
	if(current == NULL) {
		return;
	}
	// if there are two children
    bool isTwo = false;
	if(current->getRight() != NULL && current->getLeft() != NULL) {
		AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(current));
		nodeSwap(current, pred);
        isTwo = true;
	}
    int diff = 0;
    AVLNode<Key, Value>* p = current->getParent();
    if(p != NULL) {
        if(p->getLeft() == current) {
            p->setLeft(current);
            diff = 1;
        }
        else {
            p->setRight(current);
            diff = -1;
        }
    }
	// if there are no children
	if(current->getLeft() == NULL && current->getRight() == NULL) {
		if(current == this->root_) {
			this->root_ = NULL;
		}
		else if(current->getParent()->getLeft() == current) {
			current->getParent()->updateBalance(1);
            current->getParent()->setLeft(NULL);
		}
		else {
            current->getParent()->updateBalance(-1);
			current->getParent()->setRight(NULL);
		}
		// delete current;
		// current = nullptr;
		// return;
	}
	// one child, promote the child to current spot
	else if(current->getLeft() != NULL && current->getRight() == NULL) {
		current->getLeft()->setParent(current->getParent());
		if(current == this->root_) {
			this->root_ = current->getLeft();
		}
		else if(current->getParent()->getRight() == current) {
            current->getParent()->updateBalance(-1);
			current->getParent()->setRight(current->getLeft());
		}
		else {
            current->getParent()->updateBalance(1);
			current->getParent()->setLeft(current->getLeft());
		}
		// delete current;
		// current = nullptr;
		// return;
	}
	else if(current->getRight() != NULL && current->getLeft() == NULL) {
		current->getRight()->setParent(current->getParent());
		if(current == this->root_) {
			this->root_ = current->getRight();
		}
		else if(current->getParent()->getRight() == current) {
            current->getParent()->updateBalance(-1);
			current->getParent()->setRight(current->getRight());
		}
		else {
            current->getParent()->updateBalance(1);
			current->getParent()->setLeft(current->getRight());
		}
		// delete current;
		// current = nullptr;
		// return;
	}

	delete current;
	current = nullptr;
    if(!isTwo) {
        removeFix(p, diff);
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* parent, int diff) {
    if(diff == -1) {
        rotateRight(parent->getLeft());
    }
    else if(diff == 1) {
        rotateLeft(parent->getRight());
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif