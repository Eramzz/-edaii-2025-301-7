// test_link.c
void test_link_creation() {
    Link* l = linkCreate(123);
    assert(l->id == 123);
    assert(l->next == NULL);
    linkFree(l);
}
void test_link_append() {
    Link* head = NULL;
    linkAppend(&head, 1);
    linkAppend(&head, 2);
    assert(head->id == 1);
    assert(head->next->id == 2);
    linkFree(head);
}