
<template>
  <div class="dashboard">
    <app-sidebar />
    <br />
    <v-alert dense text type="success"> Welcome back {{ fname }}! </v-alert>
    <v-row>
      <v-col>
        <v-card class="blue-grey darken-3">
          <v-card-title class="cyan darken-1">
            <span
              class="font-weight-bold text-h5 blue-grey--text text--darken-3"
              >Edit profile</span
            >
          </v-card-title>
          <v-col <v-col cols="12" sm="12" >
          <v-form @submit.prevent="submitRegister" id="register-form">
            <v-row>
              <v-col cols="12" sm="5" md="5">
                <v-text-field
                  dark
                  id="firstname"
                  :label='fname'
                  name="firstname"
                  placeholder="New Name"
                  prepend-icon="person"
                  type="Firstname"
                  color="cyan darken-1"
                  autocomplete="off"
                ></v-text-field>
              </v-col>
              <v-col cols="12" sm="7" md="7">
                <v-text-field
                  dark
                  id="lastname"
                  :label='lname'
                  name="lastname"
                  placeholder="New Lastname"
                  type="Lastname"
                  color="cyan darken-1"
                  autocomplete="off"
                ></v-text-field>
              </v-col>
            </v-row>
            <v-text-field
              dark
              id="email"
              :rules="emailRules"
              :label='email'
              name="email"
              placeholder="New Email"
              prepend-icon="email"
              type="text"
              color="cyan darken-1"
              autocomplete="off"
              required
            />
            <v-text-field
              dark
              :append-icon="show1 ? 'mdi-eye' : 'mdi-eye-off'"
              :type="show1 ? 'text' : 'password'"
              id="password"
              label="Password"
              name="password"
              placeholder="New password"
              v-model="password"
              prepend-icon="lock"
              color="cyan darken-1"
              @click:append="show1 = !show1"
              required
            />
            <v-text-field
              dark
              id="phone"
              :label='phone'
              name="phone"
              placeholder="New Phone"
              prepend-icon="phone"
              type="Phone"
              color="cyan darken-1"
            />
            <v-text-field
              dark
              id="address"
              :label='address'
              name="address"
              placeholder="New Addres"
              prepend-icon="mdi-map-marker"
              type="Address"
              color="cyan darken-1"
            />
            <v-file-input
              dark
              color="cyan darken-1"
              accept="image/*"
              label="Add new profile picture"
            ></v-file-input>
            <br />
            <h5>{{ registerResponse }}</h5>
            <div class="text-center mt-n5">
              <v-btn
                type="submit"
                rounded
                color="cyan darken-1"
                dark
                form="register-form"
                >SAVE</v-btn
              >
            </div>
          </v-form>
          </v-col>
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
      fname: "",
      show1: false,
    };
  },
    methods: {
    async getUserData() {
      const response = await axios.get("/api/user/info");
      this.fname = response.data.fname;
      this.lname = response.data.lname;
      this.email = response.data.email;
      this.phone = response.data.phone;
      this.address = response.data.address;
    },
  },
    mounted() {
    this.getUserData();
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
