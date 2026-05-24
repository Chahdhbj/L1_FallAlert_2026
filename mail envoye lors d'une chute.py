import serial
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

# === CONFIG ===
PORT = "COM4"
BAUDRATE = 115200
EMAIL_FROM = "fallalert.uca@gmail.com"
EMAIL_TO = "chahdhabjiya@gmail.com"
MOT_DE_PASSE = "uhsq fods nvsx csju"

def envoyer_mail():
    msg = MIMEMultipart()
    msg["Subject"] = "⚠️ FallAlert — Chute potentielle détectée"
    msg["From"] = EMAIL_FROM
    msg["To"] = EMAIL_TO

    corps = """\
Bonjour,

Une chute potentielle de la personne concernée par votre suivi a été détectée par le système FallAlert.

Nous vous conseillons de la contacter dès que possible afin de vérifier que tout va bien.

En cas d'urgence, n'hésitez pas à contacter les secours (15 ou 18).

Merci pour votre vigilance.

Cordialement,
L'équipe FallAlert"""

    msg.attach(MIMEText(corps, "plain", "utf-8"))

    with smtplib.SMTP_SSL("smtp.gmail.com", 465) as serveur:
        serveur.login(EMAIL_FROM, MOT_DE_PASSE)
        serveur.send_message(msg)
    print("Mail envoyé !")

# === ÉCOUTE SÉRIE ===
ser = serial.Serial(PORT, BAUDRATE)
print("En attente d'alerte...")
while True:
    ligne = ser.readline().decode("utf-8").strip()
    print(ligne)
    if "ALERTE CHUTE" in ligne:
        envoyer_mail()