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

var people = [];
var counter_message;

async function send_buttons(channel) {
	const join = new ButtonBuilder()
		.setCustomId("play")
		.setLabel("Play")
		.setStyle(ButtonStyle.Success);
	const leave = new ButtonBuilder()
		.setCustomId("leave")
		.setLabel("Leave")
		.setStyle(ButtonStyle.Danger);

	const row = new ActionRowBuilder()
		.addComponents(join, leave);

	channel.send({
		content: "Press to join",
		components: [row]
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
	if (people.length == 0) {
		new_message = "No one is gaming tonight :sob:";
	} else if (people.length == 1) {
		new_message = "1 person is gaming tonight, don't leave him (scusa Sofia) alone!";
	} else {
		new_message = `${people.length} people are gaming tonight! join the party :partying_face:`;
	}
	for (let [_, person] of people) {
		new_message += `\n@${person}`;
	}
	await counter_message.edit(new_message);
}

client.on("interactionCreate", interaction => {
	if (!interaction.isButton()) return;
	if (interaction.customId == "play") {
		if (people.find(([id, _]) => id == interaction.member.id)) {
			interaction.reply({
				content: "I know you really want to game, but please calm down",
				ephemeral: true
			});
			return;
		}
		people.push([interaction.member.id, interaction.member.nickname]);
		update_counter();

		console.log(`${interaction.user.username} joined`);
		interaction.reply({
			content: `Let's go gamer!`,
			ephemeral: true
		});
	} else if (interaction.customId == "leave") {
		if (people.find(([id, _]) => id == interaction.member.id)) {
			interaction.reply({
				content: "You are already being a bad gamer",
				ephemeral: true
			});
			return;
		}

		people = people.filter(([id, _]) => id == interaction.member.id);
		update_counter();

		console.log(`${interaction.user.username} left`);
		interaction.reply({
			content: `That's sad :cry:`,
			ephemeral: true
		});
	} else {
		throw new Error("Unknown button");
	}
});

schedule.scheduleJob("0 0 0 * * *", () => {
	people = [];
	update_counter();
});

client.login(token);
