#include <Arduino.h>

template<typename T>
class CircularLinkedListNode {
public:
  T data;
  CircularLinkedListNode<T>* next;

  CircularLinkedListNode(T data)
    : data(data), next(nullptr) {}
};

template<typename T>
class CircularLinkedList {
private:
  CircularLinkedListNode<T>* head;
  CircularLinkedListNode<T>* currentNode;

public:
  CircularLinkedList()
    : head(nullptr), currentNode(nullptr) {}

  void add(T value) {
    CircularLinkedListNode<T>* newNode = new CircularLinkedListNode<T>(value);
    if (head == nullptr) {
      head = newNode;
      head->next = head;  // Circular linking
    } else {
      CircularLinkedListNode<T>* temp = head;
      while (temp->next != head) {
        temp = temp->next;
      }
      temp->next = newNode;
      newNode->next = head;  // Circular linking
    }
  }

  void remove(T value) {
    if (head == nullptr) {
      return;  // List is empty
    }

    CircularLinkedListNode<T>* current = head;
    CircularLinkedListNode<T>* previous = nullptr;
    CircularLinkedListNode<T>* deletedNode = nullptr;

    do {
      if (current->data == value) {
        if (current == head) {
          head = head->next;
          deletedNode = current;
          currentNode = head;
        } else {
          previous->next = current->next;
          deletedNode = current;
          if (current == currentNode)
            currentNode = currentNode->next;  // Update currentNode if it is being deleted
        }
        delete deletedNode;
        return;
      }
      previous = current;
      current = current->next;
    } while (current != head);

    if (head->next == head) {
      head->next = nullptr;
    }
  }

  class Iterator {
  private:
    CircularLinkedListNode<T>* current;
  public:
    Iterator(CircularLinkedListNode<T>* start)
      : current(start) {}

    T& operator*() {
      return current->data;
    }

    Iterator& operator++() {
      current = current->next;
      return *this;
    }

    bool operator!=(const Iterator& other) const {
      return current != other.current;
    }

    bool operator==(const Iterator& other) const {
      return current == other.current;
    }

    bool operator!() const {
      return current == nullptr;
    }
  };

  Iterator begin() {
    return Iterator(head);
  }

  Iterator end() {
    return Iterator(head);
  }

  Iterator next() {
    if (head == nullptr) {
      return Iterator(nullptr);
    }
    if (currentNode == nullptr) {
      currentNode = head;
      return Iterator(currentNode);
    }
    currentNode = currentNode->next;
    return Iterator(currentNode);
  }
  
  template<typename Predicate>
  Iterator find(Predicate predicate) {
    CircularLinkedListNode<T>* current = head;
    if (current == nullptr)
      return Iterator(nullptr);

    do {
      if (predicate(current->data)) {
        return Iterator(current);
      }
      current = current->next;
    } while (current != head);

    return Iterator(nullptr);  // Element not found
  }

  bool empty() const {
    return head == nullptr;
  }

  ~CircularLinkedList() {
    CircularLinkedListNode<T>* current = head;
    if (current != nullptr) {
      do {
        CircularLinkedListNode<T>* temp = current;
        current = current->next;
        delete temp;
      } while (current != head);
    }
    Serial.println("circular linked list removed");
  }
};
