/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Interrupts Implementation
 */
#include <spede/stdio.h>
#include <spede/string.h>
#include <spede/flames.h> // For inb and outb functions

#include "interrupts.h"
#include "kernel.h"

/**
 * Registers an IRQ with the Interrupt Descriptor Table (IDT)
 * @param irq_num - the IRQ number to register
 * @param isr_func - the ISR function pointer
 * @param irq_handler - the IRQ handler function pointer
 */
void interrupts_irq_register(int irq_num, void *isr_func, void *irq_handler) {
    if (irq_num < 0 || irq_num >= IDT_SIZE) {
        kernel_log_error("interrupts: invalid IRQ number: %d", irq_num);
        return;
    }

    // Set the ISR entry in the IDT
    fill_gate(&idt[irq_num], (int)isr_func, get_cs(), ACC_INTR_GATE, 0);

    // Set the IRQ handler in the array
    irq_routines[irq_num] = irq_handler;
}

/**
 * Enables interrupts via the Programmable Interrupt Controller (PIC)
 */
void pic_irq_enable() {
    outb(0x21, inb(0x21) & 0xfb); // Enable IRQ 2
    outb(0xa1, inb(0xa1) & 0xff); // Enable IRQs 8-15
}

/**
 * Disables interrupts via the Programmable Interrupt Controller (PIC)
 */
void pic_irq_disable() {
    outb(0x21, inb(0x21) | 0x04); // Disable IRQ 2
    outb(0xa1, inb(0xa1) | 0x02); // Disable IRQs 8-15
}

/**
 * Checks if a specific IRQ is enabled via the PIC
 * @param irq_num - the IRQ number to check
 * @return 1 if enabled, 0 if disabled
 */
int pic_irq_enabled(int irq_num) {
    if (irq_num < 0 || irq_num > 15) {
        kernel_log_error("interrupts: invalid IRQ number: %d", irq_num);
        return 0;
    }

    if (irq_num < 8) {
        return (inb(0x21) & (1 << irq_num)) == 0;
    } else {
        return (inb(0xa1) & (1 << (irq_num - 8))) == 0;
    }
}

/**
 * Dismisses an IRQ via the Programmable Interrupt Controller (PIC)
 * @param irq_num - the IRQ number to dismiss
 */
void pic_irq_dismiss(int irq_num) {
    if (irq_num < 0 || irq_num > 15) {
        kernel_log_error("interrupts: invalid IRQ number: %d", irq_num);
        return;
    }

    if (irq_num < 8) {
        outb(0x20, 0x20); // Send EOI to primary PIC
    } else {
        outb(0xa0, 0x20); // Send EOI to secondary PIC
        outb(0x20, 0x20); // Send EOI to primary PIC
    }
}
