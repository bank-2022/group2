<template>
  <v-row>
    <v-col cols="12">
      <v-card>
        <v-card-title class="cyan darken-1">
          <span class="font-weight-bold text-h5 blue-grey--text text--darken-3">Cards</span>
          <v-spacer></v-spacer>
          <v-icon color="blue-grey darken-3"> mdi-credit-card </v-icon>
          <span class="text-subtitle-1 blue-grey--text text--darken-3">Debit</span>
          <v-icon color="blue-grey darken-3"> mdi-credit-card-check </v-icon>
          <span class="text-subtitle-1 blue-grey--text text--darken-3">Credit</span>
        </v-card-title>
        <v-list color="blue-grey darken-3" v-for="(item, index) in cardlist" :key="index">
          <v-list-item dark>
            <v-list-item-action>
              <v-icon>{{ item.type }}</v-icon>
            </v-list-item-action>
            <v-list-item-content>
              <v-list-item-title>{{ item.title }} | Account ID: {{ item.accountId }} | {{ item.balance }}€   | <v-icon small  @click="deleteAccount(item)">mdi-delete</v-icon></v-list-item-title>
            </v-list-item-content>

          </v-list-item>
          <v-divider dark inset></v-divider>
        </v-list>
      </v-card>
    </v-col>
  </v-row>
</template>

<script>
import axios from "../axios";
export default {
  name: "Dashboard",
  data() {
    return {
      cardlist: [],
    };
  },
  methods: {
    getAccounts() {
      axios.get("/api/card/info").then((response) => {
        let ctype = null;
        for (var i = 0; i < response.data.length; i++) {
          if (response.data[i].card_type === 0) {
            ctype = "mdi-credit-card";
          } else {
            ctype = "mdi-credit-card-check";
          }
          this.cardlist.push({
            title: response.data[i].card_number,
            type: ctype,
            accountId: response.data[i].account_ID,
            balance: response.data[i].balance,
          });
        }
      });
    },
  },
  mounted() {
    this.getAccounts();
  },
};
</script>