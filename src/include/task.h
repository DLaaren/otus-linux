#ifndef TASK_H
#define TASK_H

#include <linux/list.h>
#include <linux/list_sort.h>

typedef struct {
	unsigned id;
	char name[32];
	unsigned priority;

	struct list_head node;
	// struct list_head {
	// 	struct list_head *next, *prev;
	// };
} task_entry;

typedef struct list_head * task_list;

task_list task_list_init(void);
void task_list_free(task_list tasks);

int task_list_add(task_list tasks, int id, const char *name, int priority);
int task_list_sorted_add(task_list tasks, int id, const char *name, int priority);
void task_list_print(task_list tasks);
void task_list_sort(task_list tasks);

task_list task_list_init(void)
{
	task_list tasks = kzalloc(sizeof(*tasks), GFP_KERNEL);
	INIT_LIST_HEAD(tasks);
	return tasks;
}

void task_list_free(task_list tasks)
{
    task_entry *task, *tmp;
    list_for_each_entry_safe(task, tmp, tasks, node) {
        pr_info("Freeing [ id=%d name=%s priority=%d ]\n", task->id, task->name, task->priority);
        list_del(&task->node);
        kfree(task);
    }
	kfree(tasks);
}

int task_list_add(task_list tasks, int id, const char *name, int priority)
{
	task_entry *task = kzalloc(sizeof(task_entry), GFP_KERNEL);
	if (!task)
      return -ENOMEM;

	task->id = id;
    snprintf(task->name, sizeof(task->name), "%s", name);
	task->priority = priority;
	INIT_LIST_HEAD(&task->node);

    list_add_tail(&task->node, tasks);
	pr_info("Added task [ id=%d name=%s priority=%d ]\n", task->id, task->name, task->priority);
	return 0;
}

int task_list_sorted_add(task_list tasks, int id, const char *name, int priority)
{
	task_entry *task = kzalloc(sizeof(task_entry), GFP_KERNEL);
	if (!task)
      return -ENOMEM;

	task->id = id;
    snprintf(task->name, sizeof(task->name), "%s", name);
	task->priority = priority;
	INIT_LIST_HEAD(&task->node);

	{
		task_entry *tmp;
		bool inserted = false;
		list_for_each_entry(tmp, tasks, node) {
			if (task->priority >= tmp->priority) {
				list_add_tail(&task->node, &tmp->node);
				inserted = true;
				break;
			}
		}

		if (!inserted) {
			list_add_tail(&task->node, tasks);
		}
	}
	pr_info("Added task [ id=%d name=%s priority=%d ]\n", task->id, task->name, task->priority);
	return 0;
}

void task_list_print(task_list tasks) {
  task_entry *task;
  list_for_each_entry(task, tasks, node) {
      pr_info("[ id=%d name=%s priority=%d ]\n", task->id, task->name, task->priority);
  }
}

static int task_cmp (void *priv, const struct list_head *a, const struct list_head *b)
{
	task_entry *ta = container_of(a, task_entry, node);
	task_entry *tb = container_of(b, task_entry, node);
	return tb->priority - ta->priority;
}

void task_list_sort(task_list tasks)
{
	list_sort(NULL, tasks, task_cmp);
}

#endif
