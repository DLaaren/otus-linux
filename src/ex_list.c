#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/stat.h>

#include "include/task.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DLaaren");
MODULE_DESCRIPTION(
    "Module written during the Linux development course on Otus");
MODULE_VERSION("0.1");

#define TASKS_NUM 3

task_list tasks;

static int __init ex_list_init(void) {
  pr_info("init\n");

  tasks = task_list_init();

  for (unsigned id = 0; id < TASKS_NUM; id++) {
    int ret;
    char name[32];
    unsigned priority;

    snprintf(name, sizeof(name), "Aboba%d", id);
    get_random_bytes(&priority, sizeof(priority));
    priority %= 100;

    if ((ret = task_list_add(tasks, id, name, priority)) != 0) {
      pr_err("Failed to add task [ id=%d name=%s priority=%d ]\n", id, name,
             priority);
      return ret;
    }
  }

  pr_info("Before sort:\n");
  task_list_print(tasks);

  // sort 'em out
  task_list_sort(tasks);

  pr_info("After sort:\n");
  task_list_print(tasks);

  // insert a new task according to its priority
  for (unsigned id = TASKS_NUM; id < 2 * TASKS_NUM; id++) {
    int ret;
    char name[32];
    unsigned priority;

    snprintf(name, sizeof(name), "Aboba%d", id);
    get_random_bytes(&priority, sizeof(priority));
    priority %= 100;

    if ((ret = task_list_sorted_add(tasks, id, name, priority)) != 0) {
      pr_err("Failed to add task [ id=%d name=%s priority=%d ]\n", id, name,
             priority);
      return ret;
    }
  }

  task_list_print(tasks);

  return 0;
}

static void __exit ex_list_exit(void) {
  task_list_free(tasks);
  pr_info("exit\n");
}

module_init(ex_list_init);
module_exit(ex_list_exit);
