//Classifying Prototype-based Programming Languages
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define KEY_X 1
#define KEY_Y 2
#define KEY_NAME 3
#define KEY_ADD 4

typedef struct Object * Object;
typedef struct Value * Value;
typedef Value (*Method) (Object self, Object parameter);

typedef struct Value {
    int8_t value_type;
    union {
        int64_t number;
        char * string;
        Method method;
    };
} * Value;
Value NULL_VALUE = NULL;

Value Value_int (int64_t value) {
    Value result = calloc(1, sizeof(struct Value));
    result->value_type = 1;
    result->number = value;
    return result;
}
Value Value_string (char * value) {
    Value result = calloc(1, sizeof(struct Value));
    result->value_type = 2;
    result->string = value;
    return result;
}
Value Value_method (Method value) {
    Value result = calloc(1, sizeof(struct Value));
    result->value_type = 3;
    result->method = value;
    return result;
}


void Value_free (Value value) {
    switch (value->value_type) {
        case 1:
            printf("free %ld\n", value->number);
            break;
        case 2:
            printf("free %s\n", value->string);
            break;
        case 3:
            printf("free method\n");
            break;
        default:
            break;
    }
    free(value);
}


typedef struct Slot {
    int64_t key;
    Value value;
    struct Slot *next;
} * Slot;


Slot Slot_int (int64_t key,  int64_t value) {
    Value result_value = Value_int(value);
    Slot result = calloc(1, sizeof(struct Slot));
    result->key = key;
    result->value = result_value;
    return result;
}
Slot Slot_string (int64_t key,  char * value) {
    Value result_value = Value_string(value);
    Slot result = calloc(1, sizeof(struct Slot));
    result->key = key;
    result->value = result_value;
    return result;
}
Slot Slot_method (int64_t key,  Method value) {
    Value result_value = Value_method(value);
    Slot result = calloc(1, sizeof(struct Slot));
    result->key = key;
    result->value = result_value;
    return result;
}


Slot Slot_free (Slot slot) {
    Slot next = slot->next;
    Value_free(slot->value);
    free(slot);
    return next;
}


typedef struct Object {
    Slot first;
    struct Object *parrent;
} * Object;


void Object_set(Object self, Slot slot) {
    Slot prev = NULL;
    Slot current = NULL;
    if (self->first == NULL) {
        self->first = slot;
        return;
    }
    else {
        current = self->first;
        while (current != NULL) {
            if (slot->key < current->key) {
                if (prev == NULL) {
                    self->first = slot;
                    slot->next = current;
                }
                else {
                    slot->next = prev->next;
                    prev->next = slot;
                }
                return;
            }
            else if (slot->key == current->key) {
                if (prev == NULL) {
                    self->first = slot;
                    slot->next = current->next;
                }
                else {
                    slot->next = current->next;
                    prev->next = slot;
                }
                current->next = NULL;
                Slot_free(current);
                return;
            }
            else {
                prev = current;
                current = prev->next;
            }
        }
        prev->next = slot;
    }
}


Value Object_get(Object self, int64_t key) {
    Slot current = self->first;
    while (current != NULL) {
        if (current->key == key) {
            return current->value;
        }
        current = current->next;
    }
    if (self->parrent != NULL) {
        return Object_get(self->parrent, key);
    }
    return NULL_VALUE;
}


Object Object_create(void) {
    Object self = calloc(1, sizeof(struct Object));
    self->parrent = NULL;
    self->first = NULL;
    printf("init\n");
    return self;
}


Object Object_extend(Object from) {
    Object self = Object_create();
    self->parrent = from;
    printf("extend\n");
    return self;
}


void Object_free(Object self) {
    printf("free\n");
    Slot current = self->first;
    while (current != NULL) {
        current = Slot_free(current);
    }
    free(self);
}


Value method_add_self(Object self, Object parameter) {
    int64_t x = Object_get(self, KEY_X)->number;
    int64_t y = Object_get(self, KEY_Y)->number;
    return Value_int(x + y);
}

Value method_add_parameter(Object self, Object parameter) {
    int64_t x = Object_get(parameter, KEY_X)->number;
    int64_t y = Object_get(parameter, KEY_Y)->number;
    return Value_int(x + y);
}


void test_method_with_parameter(void) {
    Object parameter = Object_create();
    Object self = Object_create();

    Object_set(parameter, Slot_int(KEY_X, 1));
    Object_set(parameter, Slot_int(KEY_Y, 2));
    Object_set(self, Slot_method(KEY_ADD, &method_add_parameter));

    Value result = Object_get(self, KEY_ADD)->method(self, parameter);
    printf("method test: %ld\n", result->number);
    Value_free(result);

    Object_free(self);
    Object_free(parameter);
}


void test_method_with_self(void) {
    Object parameter = Object_create();
    Object self = Object_create();

    Object_set(self, Slot_int(KEY_X, 1));
    Object_set(self, Slot_int(KEY_Y, 2));
    Object_set(self, Slot_method(KEY_ADD, &method_add_self));

    Value result = Object_get(self, KEY_ADD)->method(self, parameter);
    printf("method test: %ld\n", result->number);
    Value_free(result);

    Object_free(self);
    Object_free(parameter);
}


void test_extend(void) {
    Object self = Object_create();

    Object_set(self, Slot_int(KEY_X, 30));
    Object other = Object_extend(self);
    Object_set(self, Slot_int(KEY_Y, 10));

    printf("other get x: %ld\n",Object_get(other, KEY_X)->number);
    printf("other get y: %ld\n",Object_get(other, KEY_Y)->number);

    Object_free(other);
    Object_free(self);
}


void test_set_new_value(void) {
    Object self = Object_create();

    Object_set(self, Slot_int(KEY_X, 30));
    Object other = Object_extend(self);
    Object_set(self, Slot_int(KEY_Y, 10));
    Object_set(other, Slot_int(KEY_X, 20));

    printf("self get x: %ld\n",Object_get(self, KEY_X)->number);
    printf("other get x: %ld\n",Object_get(other, KEY_X)->number);
    printf("self get y: %ld\n",Object_get(self, KEY_Y)->number);
    printf("other get y: %ld\n",Object_get(other, KEY_Y)->number);

    Object_free(other);
    Object_free(self);
}


void test_string(void) {
    Object self = Object_create();

    Object_set(self, Slot_string(KEY_NAME, "hello"));
    printf("self get: %s\n",Object_get(self, KEY_NAME)->string);

    Object_free(self);
}


int main(void) {
    test_method_with_self();
    test_method_with_parameter();
    test_extend();
    test_set_new_value();
    test_string();
}
