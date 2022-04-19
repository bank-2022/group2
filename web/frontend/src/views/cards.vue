
<template>
  <div class="dashboard">
    <app-sidebar />
    <app-cardtable/>
    <br>
    <app-cards />
    <v-row>
      <v-col cols="12" sm="8">
        <v-card class="blue-grey darken-3">
          <v-card-title class="cyan darken-1">
            <span
              class="font-weight-bold text-h5 blue-grey--text text--darken-3"
              >Add card</span
            >
          </v-card-title>
          <v-form @submit.prevent="addCard" id="addcard-form">
            <v-col class="d-flex">
              <v-select
                v-model="selectAccount"
                dark
                color="cyan darken-1"
                :items="accountlist"
                item-value="id"
                item-text="name"
                label="Account"
                prepend-icon="person"
              ></v-select>
            </v-col>
            <v-col>
              <v-select
                v-model="selectType"
                dark
                color="cyan darken-1"
                :items="cardtype"
                label="Debit or Credit"
                prepend-icon="mdi-credit-card"
              ></v-select>
            </v-col>
            <v-col>
              <v-text-field
                prepend-icon="lock"
                dark
                :rules="[rules.required, rules.min]"
                :append-icon="show1 ? 'mdi-eye' : 'mdi-eye-off'"
                :type="show1 ? 'text' : 'password'"
                name="input-10-2"
                color="cyan darken-1"
                label="Pin code"
                v-model="pin"
                hint="4 Digits"
                class="input-group--focused"
                @click:append="show1 = !show1"
              ></v-text-field>
            </v-col>
            <br />
            <div class="text-center">
              <v-btn
                type="submit"
                rounded
                color="cyan darken-1"
                class="blue-grey--text text--darken-3"
                dark
                form="addcard-form">
                Add card
              </v-btn>
            </div>
            <span class="cyan--text">{{ this.addCardRes }}</span>
          </v-form>
          <br />
        </v-card>
      </v-col>
    </v-row>
  </div>
</template>

<script>
import axios from "../axios";
export default {
  data() {
    return {
      accountlist: [],
      show1: false,
      cardtype: ["Debit", "Credit"],
      selectAccount: "",
      selectType: "",
      pin: "",
      addCardRes: "",
      rules: {
        required: (value) => !!value || "Required.",
        min: (v) => v.length == 4 || "4 Digits",
      },
    };
  },
  methods: {
    getAccounts() {
      axios.get("/api/account").then((response) => {
        for (var i = 0; i < response.data.length; i++) {
          this.accountlist.push({
            id: response.data[i].account_ID,
            name: response.data[i].name,
          });
        }
      });
    },
    addCard() {
      axios.put("/api/card/connect", {
          accountId: this.selectAccount,
          card_type: this.selectType,
          pin: this.pin,
        }).then((response) => {
          this.addCardRes = response.data.message;
          if (response.data.status === "success") {
            window.location.replace("/cards");
          }
        });
    },
  },
  mounted() {
    this.getAccounts();
  },
};
</script>

<style scoped>
.overlap-icon {
  position: absolute;
  top: -33px;
  text-align: center;
  padding-top: 12px;
}
</style>
