// should be define before any #include directive
// (https://docs.kernel.org/core-api/printk-basics.html)
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DLaaren");
MODULE_DESCRIPTION(
    "Module written during the Linux development course on Otus");
MODULE_VERSION("0.1");

#define STR_BUF_LEN 24

static int idx;
static int idx_set(const char *val, const struct kernel_param *kp);
static const struct kernel_param_ops idx_ops = {
    .set = idx_set,
    .get = param_get_int,
};
module_param_cb(idx, &idx_ops, &idx, 0664);
MODULE_PARM_DESC(idx, "Index in str_buf");

static unsigned char ch_val;
static int ch_val_set(const char *val, const struct kernel_param *kp);
static int ch_val_get(char *buffer, const struct kernel_param *kp);
static const struct kernel_param_ops ch_val_ops = {
    .set = ch_val_set,
    .get = ch_val_get,
};
module_param_cb(ch_val, &ch_val_ops, &ch_val, 0664);
MODULE_PARM_DESC(ch_val, "ASCII code to write at str_buf[idx]");

static char str_buf[STR_BUF_LEN];
static int str_buf_get(char *buffer, const struct kernel_param *kp);
static const struct kernel_param_ops str_buf_ops = {
    .set = NULL, // read-only
    .get = str_buf_get,
};
module_param_cb(str_buf, &str_buf_ops, str_buf, 0444);
MODULE_PARM_DESC(str_buf, "Resulting string (read-only)");

// init function always returns int value
static int __init hello_world_init(void) {
  pr_info("init\n");
  pr_info("initial string: \"%s\"\n", str_buf);

  return 0;
}

static void __exit hello_world_exit(void) {
  pr_info("final string: \"%s\"\n", str_buf);
  pr_info("exit\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

static inline bool validate_idx(int idx) {
  return idx >= 0 && idx < STR_BUF_LEN;
}

static int idx_set(const char *val, const struct kernel_param *kp) {
  if (!validate_idx(idx)) {
    pr_err("idx %d out of range [0, %d]\n", idx, STR_BUF_LEN - 1);
    return -EINVAL;
  }

int ret = param_set_int(val, kp);
  if (ret < 0)
    return ret;
  return 0;
}

static int ch_val_set(const char *val, const struct kernel_param *kp) {
  if (!validate_idx(idx)) {
    pr_err("idx %d out of range [0, %d]\n", idx, STR_BUF_LEN - 1);
    return -EINVAL;
  }

  unsigned char ch_tmp = val[0];

  if (ch_tmp < 32 || ch_tmp > 126) {
    pr_err("ch_val %u not in printable ASCII (32-126)\n", ch_tmp);
    return -EINVAL;
  }

  ch_val = ch_tmp;
  str_buf[idx] = ch_val;
  return 0;
}

static int ch_val_get(char *buffer, const struct kernel_param *kp) {
  return sprintf(buffer, "%c", ch_val);
}

static int str_buf_get(char *buffer, const struct kernel_param *kp) {
  return sprintf(buffer, "%s", str_buf);
}
