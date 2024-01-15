const {
	Client,
	Events,
	GatewayIntentBits,
	ButtonBuilder,
	ActionRowBuilder,
	ButtonStyle
} = require("discord.js");
const { token, channel_id } = require("./config.json");
const schedule = require("node-schedule");

const client = new Client({ intents: [GatewayIntentBits.Guilds] });

var playing_today = [];
var playing_tomorrow = [];
var counter_message;

async function send_buttons(channel) {
	const today = new ActionRowBuilder()
		.addComponents(
			new ButtonBuilder()
				.setCustomId("play")
				.setLabel("Play")
				.setStyle(ButtonStyle.Success),
			new ButtonBuilder()
				.setCustomId("leave")
				.setLabel("Leave")
				.setStyle(ButtonStyle.Danger)
		);

	const tomorrow = new ActionRowBuilder()
		.addComponents(
			new ButtonBuilder()
				.setCustomId("play_t")
				.setLabel("Play tomorrow")
				.setStyle(ButtonStyle.Success),
			new ButtonBuilder()
				.setCustomId("leave_t")
				.setLabel("Leave tomorrow")
				.setStyle(ButtonStyle.Danger)
		);

	channel.send({
		content: "Press to join",
		components: [today, tomorrow]
	});
}

async function send_counter(channel) {
	counter_message = await channel.send({
		content: "No one is playing this night :sob:"
	});
}

client.once(Events.ClientReady, c => {
	console.log(`Ready! Logged in as ${c.user.tag}`);

	client.channels.fetch(channel_id)
		.then(channel => {
			send_buttons(channel)
			send_counter(channel)
		});
});

async function update_counter() {
	var new_message;
	if (playing_today.length == 0) {
		new_message = "No one is gaming tonight :sob:";
	} else if (playing_today.length == 1) {
		new_message = "1 person is gaming tonight, don't leave him (scusa Sofia) alone!";
	} else {
		new_message = `${playing_today.length} people are gaming tonight! Join the party :partying_face:`;
	}
	for (let [_, person] of playing_today) {
		new_message += `\n@${person}`;
	}

	if (playing_tomorrow.length > 0) {
		new_message += "\n\n";
		if (playing_tomorrow.length == 1) {
			new_message += "1 person will be gaming tomorrow";
		} else {
			new_message += `${playing_tomorrow.length} people will be gaming tomorrow`;
		}
		for (let [_, person] of playing_tomorrow) {
			new_message += `\n@${person}`;
		}
	}
	await counter_message.edit(new_message);
}

client.on("interactionCreate", interaction => {
	if (!interaction.isButton()) return;
	if (interaction.customId == "play") {
		if (playing_today.find(([id, _]) => id == interaction.member.id)) {
			interaction.reply({
				content: "I know you really want to game, but please, calm down",
				ephemeral: true
			});
			return;
		}
		playing_today.push([interaction.member.id, interaction.member.nickname]);
		update_counter();

		console.log(`${interaction.user.username} joined`);
		interaction.reply({
			content: "Let's go gamer!",
			ephemeral: true
		});
	} else if (interaction.customId == "leave") {
		if (!playing_today.find(([id, _]) => id == interaction.member.id)) {
			interaction.reply({
				content: "You are already being a bad gamer :angry:",
				ephemeral: true
			});
			return;
		}

		playing_today = playing_today.filter(([id, _]) => id != interaction.member.id);
		update_counter();

		console.log(`${interaction.user.username} left`);
		interaction.reply({
			content: "That's sad :cry:",
			ephemeral: true
		});
	} else if (interaction.customId == "play_t") {
		if (playing_tomorrow.find(([id, _]) => id == interaction.member.id)) {
			interaction.reply({
				content: "Tomorrow will come, be patient",
				ephemeral: true
			});
			return;
		}
		playing_tomorrow.push([interaction.member.id, interaction.member.nickname]);
		update_counter();

		console.log(`${interaction.user.username} joined tomorrow`);
		interaction.reply({
			content: "See you tomorrow!",
			ephemeral: true
		});
	} else if (interaction.customId == "leave_t") {
		if (!playing_tomorrow.find(([id, _]) => id == interaction.member.id)) {
			interaction.reply({
				content: "You have never told me you will be playing",
				ephemeral: true
			});
			return;
		}

		playing_tomorrow = playing_tomorrow.filter(([id, _]) => id != interaction.member.id);
		update_counter();

		console.log(`${interaction.user.username} left tomorrow`);
		interaction.reply({
			content: "At least you're playing today, right?",
			ephemeral: true
		});
	}else {
		throw new Error("Unknown button");
	}
});

schedule.scheduleJob("0 0 0 * * *", () => {
	console.log("Reset");
	playing_today = playing_tomorrow;
	playing_tomorrow = [];
	update_counter();
});

client.login(token);
