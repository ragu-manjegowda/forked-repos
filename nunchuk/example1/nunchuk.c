#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/input-polldev.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>

#define I2C_CHAR_BUF_SIZE 64

struct nunchuk_dev {
        struct input_polled_dev *polled_input;
        struct i2c_client *i2c_client;
};

static void nunchuk_poll(struct input_polled_dev *dev)
{
        struct nunchuk_dev *nunchuk;
        char buf[I2C_CHAR_BUF_SIZE];
        int zpressed, cpressed;
        int val;
        char empty_write;

        empty_write = 0x00;

        nunchuk = dev->private;
        if (nunchuk == NULL) {
                pr_err("Failed to get private data\n");
                goto fail;
        }

        mdelay(10);

        if ((val = i2c_master_send(nunchuk->i2c_client, &empty_write, 1)) != 1) {
                //pr_err("empty send returned error %d\n", val);
                goto fail;
        }

        mdelay(10);

        if ((val = i2c_master_recv(nunchuk->i2c_client, buf, 6)) != 6) {
                //pr_err("master_recv returned error %d\n", val);
                goto fail;
        }

        if (buf[5] & 0x01)
                zpressed = 0;
        else
                zpressed = 1;

        if (buf[5] & 0x02)
                cpressed = 0;
        else
                cpressed = 1;

        input_event(dev->input, EV_KEY, BTN_C, cpressed);
        input_event(dev->input, EV_KEY, BTN_Z, zpressed);
        input_sync(dev->input);
        return;
fail:
        //pr_err("failed\n");
        return;
}

static int nunchuk_probe(struct i2c_client *client,
        const struct i2c_device_id *id)
{
        int val;
        char buf[I2C_CHAR_BUF_SIZE];
        struct input_polled_dev *polled_input;
        struct input_dev *input;
        struct nunchuk_dev *nunchuk;

        pr_info("nunchuk_probe called\n");

        nunchuk = devm_kzalloc(&client->dev,
                sizeof(struct nunchuk_dev),
                GFP_KERNEL);
        if (!nunchuk) {
                dev_err(&client->dev, "Failed to allocate memory\n");
                return -ENOMEM;
        }

        polled_input = input_allocate_polled_device();
        nunchuk->i2c_client = client;
        nunchuk->polled_input = polled_input;
        polled_input->private = nunchuk;
        polled_input->poll = nunchuk_poll;
        polled_input->poll_interval = 500;

        i2c_set_clientdata(client, nunchuk);
        input = polled_input->input;
        input->dev.parent = &client->dev;
        input->name = "Wii Nunchuk";
        input->id.bustype = BUS_I2C;

        set_bit(EV_KEY, input->evbit);
        set_bit(BTN_C, input->keybit);
        set_bit(BTN_Z, input->keybit);

        if (input_register_polled_device(polled_input) < 0)
        {
                input_free_polled_device(polled_input);
                pr_err("failed to register polled device\n");
                return -1;
        }

        buf[0] = 0xf0;
        buf[1] = 0x55;
        if ((val = i2c_master_send(client, buf, 2)) != 2)
                pr_err("master_send returned error %d\n", val);
        mdelay(1);
        buf[0] = 0xfb;
        buf[1] = 0x00;
        if ((val = i2c_master_send(client, buf, 2)) != 2)
                pr_err("master_send returned error %d\n", val);

        return 0;
}

static int nunchuk_remove(struct i2c_client *client)
{
        struct input_polled_dev *polled_input;
        struct nunchuk_dev *nunchuk;

        pr_info("nunchuk_remove called\n");

        nunchuk = (struct nunchuk_dev*) i2c_get_clientdata(client);
        polled_input = nunchuk->polled_input;
        input_free_polled_device(polled_input);
        return 0;
}

static const struct i2c_device_id nunchuk_ids[] = {
        { "nunchuk", 0 },
        {}
};
MODULE_DEVICE_TABLE(i2c, nunchuk_ids);

static const struct of_device_id nunchuk_dt_ids[] = {
        { .compatible = "nintendo,nunchuk", },
        {}
};

static struct i2c_driver nunchuk_driver = {
        .driver = {
                .name = "nunchuk",
                .owner = THIS_MODULE,
                .of_match_table = nunchuk_dt_ids,
        },
        .id_table = nunchuk_ids,
        .probe = nunchuk_probe,
        .remove = nunchuk_remove,
};

module_i2c_driver(nunchuk_driver);
MODULE_LICENSE("GPL");
