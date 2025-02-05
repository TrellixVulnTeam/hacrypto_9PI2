Require Import floyd.proofauto.
Require Import sha.sha.
Require Import sha.SHA256.
Require Import sha.spec_sha.
Require Import sha.sha_lemmas.
Require Import sha.bdo_lemmas.
Local Open Scope logic.

Lemma rearrange_aux:
 forall h f c k l,
 Int.add (Int.add (Int.add (Int.add h f) c) k) l =
Int.add (Int.add (Int.add (Int.add l h) f) c) k.
Proof.
intros.
rewrite <- (Int.add_commut l).
repeat rewrite (Int.add_assoc h).
rewrite <- (Int.add_assoc l).
repeat rewrite (Int.add_assoc (Int.add l h)).
reflexivity.
Qed.

Lemma loop1_aux_lemma1:
forall sh b i N,
  length b = N ->
  (i < N)%nat ->
 array_at tuint sh
       (upd (f_upto (tuints b) (Z.of_nat i)) (Z.of_nat i)
          (Vint
             (big_endian_integer
                (fun z : Z =>
                 force_int
                   (tuchars (map Int.repr (intlist_to_Zlist b))
                      (z + Z.of_nat i * 4))))))
        0 (Z.of_nat N) =
  array_at tuint sh
               (f_upto (tuints b) (Z.of_nat (S i))) 
              0 (Z.of_nat N).
Proof.
intros.
apply array_at_ext; intros.
unfold upd.
if_tac.
assert (exists w, nth_error b i = Some w).
subst N.
clear - H0.
revert b H0; induction i; destruct b; simpl; intros; auto.
omega.
exists i; reflexivity. 
inv H0.
apply IHi; auto. clear - H0.
apply lt_S_n; auto.
destruct H3 as [w H3].
unfold tuchars;
rewrite <- nth_big_endian_integer with (w:=w); auto.
unfold f_upto.
subst.
rewrite if_true by (rewrite inj_S; omega).
unfold tuints, ZnthV. rewrite if_false by omega. rewrite Nat2Z.id.
clear - H3; revert b H3; induction i; destruct b; intros; inv H3; simpl; auto.
unfold f_upto.
if_tac. rewrite if_true by (rewrite inj_S; omega); auto.
rewrite if_false; auto.
rewrite inj_S; 
omega.
Qed.

Lemma split_offset_array_at: forall (ty : type) (sh : Share.t) (contents : Z -> reptype ty)
         (z lo hi : Z) (v : val),
       z <= lo <= hi ->
       sizeof ty > 0 ->
       legal_alignas_type ty = true ->
       array_at ty sh contents z hi v =
       !! offset_in_range (sizeof ty * z) v &&
       !! offset_in_range (sizeof ty * hi) v &&
       array_at ty sh contents z lo v * 
       array_at ty sh (fun i => contents (i + lo)) 0 (hi - lo) 
       (offset_val (Int.repr (sizeof ty * lo)) v).
Proof.
  intros.
  assert (~ offset_strict_in_range (sizeof ty * lo) v \/
          offset_strict_in_range (sizeof ty * lo) v) by
    (unfold offset_strict_in_range; destruct v; auto; omega).
  destruct H2.
  + rewrite (add_andp (array_at ty sh contents z hi v) (!!offset_in_range (sizeof ty * hi) v)) by 
      (unfold array_at; normalize).
    rewrite (add_andp (array_at ty sh contents z hi v) (!!offset_in_range (sizeof ty * z) v)) by 
      (unfold array_at; normalize).
    rewrite andp_assoc.
    rewrite andp_comm.
    normalize.
    apply pick_prop_from_eq; intros.
    assert (lo = hi).
      assert (offset_in_range (sizeof ty * lo) v).
      apply offset_in_range_mid with (lo := z) (hi := hi); [omega | auto |auto].
      tauto.
      tauto.
      assert ((sizeof ty * lo)%Z <= (sizeof ty * hi)%Z) by (apply Z.mul_le_mono_nonneg_l; omega).
      unfold offset_in_range, offset_strict_in_range in *; destruct v; try tauto.
      apply (Z.mul_cancel_l) with (p := sizeof ty); omega.
    subst.
    replace (hi - hi) with 0 by omega.
    rewrite array_at_emp.
    unfold array_at; apply pred_ext; normalize.
    apply andp_right; [apply prop_right | apply derives_refl].
    unfold offset_in_range, offset_strict_in_range in *; destruct v; try tauto.
    unfold offset_val, Int.add, align_compatible.
    pose proof Int.unsigned_range i.
    rewrite !Int.unsigned_repr_eq.
    rewrite Zplus_mod_idemp_r.
    assert ((Int.unsigned i + sizeof ty * hi) = Int.modulus) by omega.
    rewrite H8.
    rewrite Z_mod_same_full.
    repeat split; auto; try omega.
    apply Z.divide_0_r.
  + rewrite split_array_at with (i := lo) (lo := z) (hi := hi) by omega.
    rewrite <- offset_val_array_at with (lo := 0) (hi := hi - lo) (ofs := lo) by assumption.
    rewrite (add_andp (array_at ty sh contents lo hi v) (!!offset_in_range (sizeof ty * hi) v)) by 
      (unfold array_at; normalize).
    rewrite (add_andp (array_at ty sh contents z lo v) (!!offset_in_range (sizeof ty * z) v)) at 1 by 
      (unfold array_at; normalize).
    normalize.
    f_equal.
    f_equal.
    f_equal; [| omega].
    extensionality.
    f_equal.
    omega.
Qed.

Lemma read32_reversed_in_bytearray:
 forall {Espec: OracleKind} Delta (ofs: int) (lo hi: Z) base e sh (contents: Z -> val) i P Q
 (VS:  (var_types Delta) ! ___builtin_read32_reversed = None) 
 (GS: (glob_types Delta) ! ___builtin_read32_reversed =
    Some (Global_func (snd __builtin_read32_reversed_spec)))
 (TE: typeof e = tptr tuint)
 (TCi:  tc_fn_return Delta (Some i) tuint)
 (CLOQ: Forall (closed_wrt_vars (eq i)) Q)
 (Hcontents: forall i, (lo <= i < hi)%Z -> is_int (contents i)),
 PROPx P (LOCALx (tc_environ Delta :: Q) (SEP (TT))) |-- PROP ((lo <= Int.unsigned ofs <= hi-4 )%Z)
         LOCAL (tc_expr Delta e; `(eq (offset_val ofs base)) (eval_expr e))
         SEP  (TT) ->
 semax Delta  (PROPx P (LOCALx Q (SEP (`(array_at tuchar sh contents lo hi base)))))
        (Scall (Some i)
           (Evar ___builtin_read32_reversed
              (Tfunction (Tcons (tptr tuint) Tnil) tuint cc_default))
           [e])
        (normal_ret_assert
         (PROP () 
         (LOCALx (`(eq (Vint (big_endian_integer (fun z => force_int (contents (z+Int.unsigned ofs)%Z))))) (eval_id i)
                        :: Q)                 
         SEP (`(array_at tuchar sh contents lo hi base))))).
Proof.
intros.
apply semax_pre with
 (PROP  ((lo <= Int.unsigned ofs <= hi - 4)%Z)
        (LOCALx (tc_expr Delta e :: `(eq (offset_val ofs base)) (eval_expr e) :: Q)
         (SEP(`(array_at tuchar sh contents lo hi base))))).
rewrite <- (andp_dup (PROPx P _)).
eapply derives_trans; [ apply andp_derives | ].
eapply derives_trans; [ | apply H].
apply andp_derives; auto.
apply andp_derives; auto.
go_lowerx.
apply sepcon_derives; auto.
apply TT_right.
instantiate (1:= PROPx nil (LOCALx Q (SEP  (`(array_at tuchar sh contents lo hi base))))).
go_lowerx. entailer.
go_lowerx; entailer.
normalize.
clear H.
normalize.
rewrite (split_array_at (Int.unsigned ofs + 4)%Z) with (hi:=hi) by omega.
rewrite split_offset_array_at with (lo := Int.unsigned ofs) (hi := Int.unsigned ofs + 4);
  [| omega | simpl; omega | reflexivity].
normalize.

match goal with |- semax _ _ _ (normal_ret_assert (PROPx _ (LOCALx _ ?A))) =>
 replace A  with (SEPx( [
         `(array_at tuchar sh
             (fun i0 : Z => contents (i0 + Int.unsigned ofs)) 0
             (Int.unsigned ofs + 4 - Int.unsigned ofs)
             (offset_val (Int.repr (Int.unsigned ofs)) base))] ++
               [`(array_at tuchar sh contents lo (Int.unsigned ofs) base),
         `(array_at tuchar sh contents (Int.unsigned ofs + 4) hi base)]))
 by (simpl app; apply pred_ext; go_lowerx; normalize; cancel)
end.

match goal with |- semax _ (PROPx _ (LOCALx _ ?A)) _ _ =>
 replace A  with (SEPx( [
         `(array_at tuchar sh
             (fun i0 : Z => contents (i0 + Int.unsigned ofs)) 0
             (Int.unsigned ofs + 4 - Int.unsigned ofs)
             (offset_val (Int.repr (Int.unsigned ofs)) base))] ++
               [`(array_at tuchar sh contents lo (Int.unsigned ofs) base),
         `(array_at tuchar sh contents (Int.unsigned ofs + 4) hi base)]))
 by (simpl app; apply pred_ext; go_lowerx; normalize; cancel)
end.

eapply semax_frame1; try reflexivity;
 [ |  apply derives_refl | auto 50 with closed].
eapply semax_pre_post.

Focus 3.
evar (tl: typelist).
replace (Tcons (tptr tuint) Tnil) with tl.
unfold tl.
eapply semax_call_id1'; try eassumption.
2: reflexivity.
apply GS.
2: reflexivity.
Unfocus.

instantiate (3:=nil).
apply andp_left2.
instantiate (2:=(offset_val ofs base, sh, fun z => force_int (contents (z + Int.unsigned ofs)%Z))).
cbv beta iota.
instantiate (1:=nil).
unfold split; simpl @snd.
go_lowerx.
entailer.
apply andp_right; [apply prop_right; repeat split; auto | ].
hnf. simpl.
rewrite TE.
repeat rewrite denote_tc_assert_andp; repeat split; auto.
rewrite <- H5.
rewrite TE.
destruct base; inv Pbase; reflexivity.
pattern ofs at 4;
 replace ofs with (Int.repr (sizeof tuchar * Int.unsigned ofs))%Z
 by (simpl sizeof; rewrite Z.mul_1_l; apply Int.repr_unsigned).
(*rewrite <- offset_val_array_at.*)
apply derives_refl'.
rewrite Int.repr_unsigned.
replace (Int.unsigned ofs + 4 - Int.unsigned ofs) with 4 by omega.
simpl sizeof. rewrite Zmult_1_l. rewrite Int.repr_unsigned.
apply equal_f. apply array_at_ext.
intros. unfold cVint.

specialize (Hcontents (i0 + Int.unsigned ofs)).
destruct (contents (i0 + Int.unsigned ofs)); 
  try solve [elimtype False; apply Hcontents; omega]. reflexivity.

intros; apply andp_left2.
apply normal_ret_assert_derives'.
go_lowerx.
entailer.
apply derives_refl'.
pattern ofs at 2;
 replace ofs with (Int.repr (sizeof tuchar * Int.unsigned ofs))%Z
 by (simpl sizeof; rewrite Z.mul_1_l; apply Int.repr_unsigned).
rewrite Int.repr_unsigned.
replace (Int.unsigned ofs + 4 - Int.unsigned ofs) with 4 by omega.
simpl sizeof. rewrite Zmult_1_l. rewrite Int.repr_unsigned.
apply equal_f. 
apply array_at_ext; intros.
unfold cVint.
specialize (Hcontents (i0 + Int.unsigned ofs)).
destruct (contents (i0 + Int.unsigned ofs)); 
  try reflexivity;  contradiction Hcontents; omega.
auto.
Qed.

Definition block_data_order_loop1 := 
   nth 0 (loops (fn_body f_sha256_block_data_order)) Sskip.

Lemma sha256_block_data_order_loop1_proof:
  forall (Espec : OracleKind) (sh: share)
     (b: list int) ctx (data: val) (regs: list int)
     (Hregs: length regs = 8%nat)
     (Hdata: isptr data),
     length b = LBLOCK ->
     semax Delta_loop1
  (PROP ()
   LOCAL (`(eq ctx) (eval_id _ctx);
               `(eq (Vint (Int.repr 0))) (eval_id _i);
               `(eq data) (eval_id _data);
               `(eq (Vint (nthi regs 0))) (eval_id _a);
               `(eq (Vint (nthi regs 1))) (eval_id _b);
               `(eq (Vint (nthi regs 2))) (eval_id _c);
               `(eq (Vint (nthi regs 3))) (eval_id _d);
               `(eq (Vint (nthi regs 4))) (eval_id _e);
               `(eq (Vint (nthi regs 5))) (eval_id _f);
               `(eq (Vint (nthi regs 6))) (eval_id _g);
               `(eq (Vint (nthi regs 7))) (eval_id _h))
   SEP ( `K_vector (eval_var _K256 (tarray tuint 64));
           `(array_at_ tuint Tsh 0 16) (eval_var _X (tarray tuint 16));
           `(data_block sh (intlist_to_Zlist b) data)))
  block_data_order_loop1
  (normal_ret_assert
    (PROP () 
     LOCAL(`(eq ctx) (eval_id _ctx);
                `(eq (Vint (Int.repr 16))) (eval_id _i);
               `(eq (Vint (nthi (Round regs (nthi b) (LBLOCKz - 1)) 0))) (eval_id _a);
               `(eq (Vint (nthi (Round regs (nthi b) (LBLOCKz - 1)) 1))) (eval_id _b);
               `(eq (Vint (nthi (Round regs (nthi b) (LBLOCKz - 1)) 2))) (eval_id _c);
               `(eq (Vint (nthi (Round regs (nthi b) (LBLOCKz - 1)) 3))) (eval_id _d);
               `(eq (Vint (nthi (Round regs (nthi b) (LBLOCKz - 1)) 4))) (eval_id _e);
               `(eq (Vint (nthi (Round regs (nthi b) (LBLOCKz - 1)) 5))) (eval_id _f);
               `(eq (Vint (nthi (Round regs (nthi b) (LBLOCKz - 1)) 6))) (eval_id _g);
               `(eq (Vint (nthi (Round regs (nthi b) (LBLOCKz - 1)) 7))) (eval_id _h))
     SEP (`K_vector (eval_var _K256 (tarray tuint 64));
           `(array_at tuint Tsh (tuints b) 0 16) (eval_var _X (tarray tuint 16));
           `(data_block sh (intlist_to_Zlist b) data))) ).
Proof. {
unfold block_data_order_loop1.
intros.
simpl nth; fold rearrange_regs.
abbreviate_semax.
name a_ _a.
name b_ _b.
name c_ _c.
name d_ _d.
name e_ _e.
name f_ _f.
name g_ _g.
name h_ _h.
name l_ _l.
name l'_ _l'.
name Ki _Ki.
name in_ _in.
name ctx_ _ctx.
name i_ _i.
name data_ _data.
assert (LBE := LBLOCK_zeq).

Definition loop1_inv (rg0: list int) (sh: share) (b: list int) ctx (data: val) (delta: Z) (i: nat) :=
    PROP ( (i <= 16)%nat )
    LOCAL  (`(eq ctx) (eval_id _ctx);
                `(eq (Vint (Int.repr (Z.of_nat i - delta)))) (eval_id _i);
                `(eq (offset_val (Int.repr ((Z.of_nat i)*4)) data)) (eval_id _data);
     `(eq (Vint (nthi (Round rg0 (nthi b) (Z.of_nat i - 1)) 0))) (eval_id _a);
     `(eq (Vint (nthi (Round rg0 (nthi b) (Z.of_nat i - 1)) 1))) (eval_id _b);
     `(eq (Vint (nthi (Round rg0 (nthi b) (Z.of_nat i - 1)) 2))) (eval_id _c);
     `(eq (Vint (nthi (Round rg0 (nthi b) (Z.of_nat i - 1)) 3))) (eval_id _d);
     `(eq (Vint (nthi (Round rg0 (nthi b) (Z.of_nat i - 1)) 4))) (eval_id _e);
     `(eq (Vint (nthi (Round rg0 (nthi b) (Z.of_nat i - 1)) 5))) (eval_id _f);
     `(eq (Vint (nthi (Round rg0 (nthi b) (Z.of_nat i - 1)) 6))) (eval_id _g);
     `(eq (Vint (nthi (Round rg0 (nthi b) (Z.of_nat i - 1)) 7))) (eval_id _h))
     SEP (`K_vector (eval_var _K256 (tarray tuint 64));
    `(array_at tuint Tsh (f_upto (tuints b) (Z.of_nat i) ) 0 LBLOCKz) (eval_var _X (tarray tuint 16));
   `(data_block sh (intlist_to_Zlist b) data)).

apply semax_pre with (EX i:nat, loop1_inv regs sh b ctx data 0 i). 
(* 345,184   326,392*) {
 apply exp_right with 0%nat.
 unfold loop1_inv.
 rewrite array_at_f_upto_lo.
 replace (Round regs (nthi b) (Z.of_nat 0 - 1)) with regs.
 entailer!.
 rewrite Round_equation. rewrite if_true by (compute; auto). auto.
}
(* 419,452   431,980 *)
apply semax_post' with (loop1_inv regs sh b ctx data 0 LBLOCK).
(* 419,452  431,980 *)
  unfold loop1_inv. entailer!.
(* 445,728  479,964 *)
clear POSTCONDITION.
apply (semax_loop _ _ (EX i:nat, loop1_inv regs sh b ctx data 1 i)).
2: abstract (
apply extract_exists_pre; intro i;
forward;  (*  i += 1; *)
apply exp_right with i;
(* 452,280  481,476 *)
 unfold loop1_inv;  entailer! ; f_equal; omega).
(* 561,900  574,392 *)

apply extract_exists_pre; intro i.
unfold loop1_inv.
forward_if (
   PROP  ((i < 16)%nat)
   LOCAL  (`(eq ctx) (eval_id _ctx); 
                `(eq (Vint (Int.repr (Z.of_nat (0 + i))))) (eval_id _i);
               `(eq (offset_val (Int.repr ((Z.of_nat i)*4)) data)) (eval_id _data);
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 0))) (eval_id _a);
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 1))) (eval_id _b);
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 2))) (eval_id _c);
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 3))) (eval_id _d);
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 4))) (eval_id _e);
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 5))) (eval_id _f);
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 6))) (eval_id _g);
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 7))) (eval_id _h))
   SEP 
   (`K_vector (eval_var _K256 (tarray tuint 64));
   `(array_at tuint Tsh (f_upto (tuints b) (Z.of_nat i)) 0 LBLOCKz) (eval_var _X (tarray tuint 16));
   `(data_block sh (intlist_to_Zlist b) data))).
(* 587,640  592,608 *)
(* rewrite <- insert_local; apply andp_left1. entailer. *)
(* 613,416  655,716 *)
abstract (forward; (* skip; *)
(* 619,968  655,716 *)
   entailer; apply prop_right; rewrite Z.sub_0_r; auto).
(* 726,056  709,784 *) 
abstract (
   apply semax_extract_PROP; intro;
   forward; (* break; *)
   cbv beta;
   rewrite overridePost_EK_break,
       loop1_ret_assert_EK_break,
        normal_ret_assert_elim;
   entailer;
   assert (i=16)%nat by omega; subst i;
   apply andp_right; [ apply prop_right | cancel];
   change 16%nat with LBLOCK in H3;
   repeat split; auto).
(* 854,860 750,176 *)
unfold MORE_COMMANDS, POSTCONDITION, abbreviate;
 clear MORE_COMMANDS POSTCONDITION.
make_sequential.
unfold loop1_ret_assert.
apply extract_exists_post with (S i).
rewrite inj_S.
simpl plus.
normalize.

(* 945,760 834,556 *)
do 2 apply -> seq_assoc.
eapply semax_frame_seq
 with (P1 := [])
         (Q1 :=  [ `(eq ctx) (eval_id _ctx),
`(eq (Vint (Int.repr (Z.of_nat i)))) (eval_id _i),
`(eq (offset_val (Int.repr (Z.of_nat i * 4)) data)) (eval_id _data),
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 0))) (eval_id _a),
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 1))) (eval_id _b),
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 2))) (eval_id _c),
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 3))) (eval_id _d),
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 4))) (eval_id _e),
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 5))) (eval_id _f),
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 6))) (eval_id _g),
   `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 7))) (eval_id _h)])
         (Frame := [`K_vector (eval_var _K256 (tarray tuint 64)),
   `(array_at tuint Tsh (f_upto (tuints b) (Z.of_nat i)) 0 LBLOCKz) (eval_var _X (tarray tuint 16))]); 
   [apply (read32_reversed_in_bytearray _ (Int.repr (Z.of_nat i * 4)) 0 (Zlength (intlist_to_Zlist b)) data _ sh 
                     (tuchars (map Int.repr (intlist_to_Zlist b))));
    [ reflexivity | reflexivity | reflexivity | reflexivity
    | auto 50 with closed 
    |  intros; apply ZnthV_map_Vint_is_int; rewrite Zlength_correct, map_length;
          rewrite Zlength_correct in H1; apply H1
      | ]
   | | | ].
(* 945,760 834,556 *)
abstract solve [entailer!; repeat split; auto; try omega; 
 rewrite Zlength_correct; rewrite length_intlist_to_Zlist; 
 rewrite H;
 replace (Z.of_nat (4 * LBLOCK) - 4)%Z
  with ((Z.of_nat LBLOCK - 1) * 4)%Z; 
    [apply Zmult_le_compat_r; omega | ];
 rewrite Zmult_comm;
 rewrite Z.mul_sub_distr_l;
 reflexivity].
(* 990,216 849,172 *)
simpl app.
unfold data_block.
rewrite prop_true_andp by apply isbyte_intlist_to_Zlist.
abstract solve [entailer!].
(* 1,078,128 849,172 *)
auto 50 with closed.
simpl app.
replace (array_at tuchar sh (tuchars (map Int.repr (intlist_to_Zlist b))) 0
        (Zlength (intlist_to_Zlist b)) data)
  with (data_block sh (intlist_to_Zlist b) data) 
 by (unfold data_block;
       rewrite prop_true_andp by apply isbyte_intlist_to_Zlist;
       reflexivity).
(*

1 focused subgoals (unfocused: 0)
, subgoal 1 (ID 22116)
  
  Espec : OracleKind
  sh : share
  b : list int
  ctx : val
  data : val
  regs : list int
  Hregs : length regs = 8%nat
  Hdata : isptr data
  H : length b = LBLOCK
  a_ : name _a
  b_ : name _b
  c_ : name _c
  d_ : name _d
  e_ : name _e
  f_ : name _f
  g_ : name _g
  h_ : name _h
  l_ : name _l
  l'_ : name _l'
  Ki : name _Ki
  in_ : name _in
  ctx_ : name _ctx
  i_ : name _i
  data_ : name _data
  LBE : Z.of_nat LBLOCK = 16
  i : nat
  Delta := abbreviate : tycontext
  H0 : (i < 16)%nat
  ============================
   semax
     (update_tycon Delta
        (Scall (Some _l')
           (Evar ___builtin_read32_reversed
              (Tfunction (Tcons (tptr tuint) Tnil) tuint cc_default))
           [Ecast (Etempvar _data (tptr tuchar)) (tptr tuint)]))
     (PROP  ()
      LOCAL 
      (`(eq
           (Vint
              (big_endian_integer
                 (fun z : Z =>
                  force_int
                    (tuchars (map Int.repr (intlist_to_Zlist b))
                       (z + Int.unsigned (Int.repr (Z.of_nat i * 4))))))))
         (eval_id _l'); `(eq ctx) (eval_id _ctx);
      `(eq (Vint (Int.repr (Z.of_nat i)))) (eval_id _i);
      `(eq (offset_val (Int.repr (Z.of_nat i * 4)) data)) (eval_id _data);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 0)))
        (eval_id _a);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 1)))
        (eval_id _b);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 2)))
        (eval_id _c);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 3)))
        (eval_id _d);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 4)))
        (eval_id _e);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 5)))
        (eval_id _f);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 6)))
        (eval_id _g);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 7)))
        (eval_id _h))
      SEP  (`(data_block sh (intlist_to_Zlist b) data);
      `K_vector (eval_var _K256 (tarray tuint 64));
      `(array_at tuint Tsh (f_upto (tuints b) (Z.of_nat i)) 0 LBLOCKz)
        (eval_var _X (tarray tuint 16))))
     (Ssequence (Sset _l (Ecast (Etempvar _l' tuint) tuint))
        (Ssequence
           (Sset _data
              (Ebinop Oadd (Etempvar _data (tptr tuchar))
                 (Econst_int (Int.repr 4) tint) (tptr tuchar)))
           (Ssequence
              (Sassign
                 (Ederef
                    (Ebinop Oadd (Evar _X (tarray tuint 16))
                       (Etempvar _i tint) (tptr tuint)) tuint)
                 (Etempvar _l tuint))
              (Ssequence
                 (Sset _Ki
                    (Ederef
                       (Ebinop Oadd (Evar _K256 (tarray tuint 64))
                          (Etempvar _i tint) (tptr tuint)) tuint))
                 rearrange_regs))))
     (normal_ret_assert
        (PROP  ((S i <= 16)%nat)
         LOCAL  (`(eq ctx) (eval_id _ctx);
         `(eq (Vint (Int.repr (Z.succ (Z.of_nat i) - 1)))) (eval_id _i);
         `(eq (offset_val (Int.repr (Z.succ (Z.of_nat i) * 4)) data))
           (eval_id _data);
         `(eq (Vint (nthi (Round regs (nthi b) (Z.succ (Z.of_nat i) - 1)) 0)))
           (eval_id _a);
         `(eq (Vint (nthi (Round regs (nthi b) (Z.succ (Z.of_nat i) - 1)) 1)))
           (eval_id _b);
         `(eq (Vint (nthi (Round regs (nthi b) (Z.succ (Z.of_nat i) - 1)) 2)))
           (eval_id _c);
         `(eq (Vint (nthi (Round regs (nthi b) (Z.succ (Z.of_nat i) - 1)) 3)))
           (eval_id _d);
         `(eq (Vint (nthi (Round regs (nthi b) (Z.succ (Z.of_nat i) - 1)) 4)))
           (eval_id _e);
         `(eq (Vint (nthi (Round regs (nthi b) (Z.succ (Z.of_nat i) - 1)) 5)))
           (eval_id _f);
         `(eq (Vint (nthi (Round regs (nthi b) (Z.succ (Z.of_nat i) - 1)) 6)))
           (eval_id _g);
         `(eq (Vint (nthi (Round regs (nthi b) (Z.succ (Z.of_nat i) - 1)) 7)))
           (eval_id _h))
         SEP  (`K_vector (eval_var _K256 (tarray tuint 64));
         `(array_at tuint Tsh (f_upto (tuints b) (Z.succ (Z.of_nat i))) 0
             LBLOCKz) (eval_var _X (tarray tuint 16));
         `(data_block sh (intlist_to_Zlist b) data))))

*)

forward. (* l := l'; *)
forward. (* data := data + 4; *)
(* 1,194,800 849,172 *)

abstract solve [entailer!].
normalize.
(*
1 focused subgoals (unfocused: 0)
, subgoal 1 (ID 24357)
  
  Espec : OracleKind
  sh : share
  b : list int
  ctx : val
  data : val
  regs : list int
  Hregs : length regs = 8%nat
  Hdata : isptr data
  H : length b = LBLOCK
  a_ : name _a
  b_ : name _b
  c_ : name _c
  d_ : name _d
  e_ : name _e
  f_ : name _f
  g_ : name _g
  h_ : name _h
  l_ : name _l
  l'_ : name _l'
  Ki : name _Ki
  in_ : name _in
  ctx_ : name _ctx
  i_ : name _i
  data_ : name _data
  LBE : Z.of_nat LBLOCK = 16
  i : nat
  H0 : (i < 16)%nat
  Delta := abbreviate : tycontext
  POSTCONDITION := abbreviate : ret_assert
  MORE_COMMANDS := abbreviate : statement
  ============================
   semax Delta
     (PROP  ()
      LOCAL 
      (`(eq (offset_val (Int.repr (Z.of_nat i * 4 + 4)) data))
         (eval_id _data);
      `(eq
          (Vint
             (big_endian_integer
                (fun z : Z =>
                 force_int
                   (tuchars (map Int.repr (intlist_to_Zlist b))
                      (z + Z.of_nat i * 4)))))) (eval_id _l);
      `(eq
          (Vint
             (big_endian_integer
                (fun z : Z =>
                 force_int
                   (tuchars (map Int.repr (intlist_to_Zlist b))
                      (z + Z.of_nat i * 4)))))) (eval_id _l');
      `(eq ctx) (eval_id _ctx);
      `(eq (Vint (Int.repr (Z.of_nat i)))) (eval_id _i);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 0)))
        (eval_id _a);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 1)))
        (eval_id _b);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 2)))
        (eval_id _c);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 3)))
        (eval_id _d);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 4)))
        (eval_id _e);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 5)))
        (eval_id _f);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 6)))
        (eval_id _g);
      `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 7)))
        (eval_id _h))
      SEP  (`(data_block sh (intlist_to_Zlist b) data);
      `K_vector (eval_var _K256 (tarray tuint 64));
      `(array_at tuint Tsh (f_upto (tuints b) (Z.of_nat i)) 0 LBLOCKz)
        (eval_var _X (tarray tuint 16))))
     (Ssequence
        (Sassign
           (Ederef
              (Ebinop Oadd (Evar _X (tarray tuint 16)) 
                 (Etempvar _i tint) (tptr tuint)) tuint) 
           (Etempvar _l tuint)) MORE_COMMANDS) POSTCONDITION
*)
(* 1,291,784 894,136 *)
forward. (* X[i]=l; *)

(*
2 focused subgoals (unfocused: 0)
, subgoal 1 (ID 26501)
  
  Espec : OracleKind
  sh : share
  b : list int
  data : val
  regs : list int
  Hregs : length regs = 8%nat
  Hdata : isptr data
  H : length b = LBLOCK
  a_ : name _a
  b_ : name _b
  c_ : name _c
  d_ : name _d
  e_ : name _e
  f_ : name _f
  g_ : name _g
  h_ : name _h
  l_ : name _l
  l'_ : name _l'
  Ki : name _Ki
  in_ : name _in
  ctx_ : name _ctx
  i_ : name _i
  data_ : name _data
  LBE : Z.of_nat LBLOCK = 16
  i : nat
  H0 : (i < 16)%nat
  Delta := abbreviate : tycontext
  MORE_COMMANDS := abbreviate : statement
  rho : environ
  H1 : offset_val (Int.repr (Z.of_nat i * 4 + 4)) data = eval_id _data rho
  POSTCONDITION := abbreviate : ret_assert
  H13 : Forall isbyteZ (intlist_to_Zlist b)
  H14 : isptr (eval_var _X (tarray tuint 16) rho)
  H2 : Vint
         (big_endian_integer
            (fun z : Z =>
             force_int
               (tuchars (map Int.repr (intlist_to_Zlist b))
                  (z + Z.of_nat i * 4)))) = eval_id _l rho
  H3 : Vint
         (big_endian_integer
            (fun z : Z =>
             force_int
               (tuchars (map Int.repr (intlist_to_Zlist b))
                  (z + Z.of_nat i * 4)))) = eval_id _l' rho
  H4 : Vint (Int.repr (Z.of_nat i)) = eval_id _i rho
  H5 : Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 0) = eval_id _a rho
  H6 : Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 1) = eval_id _b rho
  H7 : Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 2) = eval_id _c rho
  H8 : Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 3) = eval_id _d rho
  H9 : Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 4) = eval_id _e rho
  H10 : Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 5) = eval_id _f rho
  H11 : Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 6) = eval_id _g rho
  H12 : Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 7) = eval_id _h rho
  ============================
   data_block sh (intlist_to_Zlist b) data *
   K_vector (eval_var _K256 (tarray tuint 64) rho) *
   array_at_ tuint Tsh 0 LBLOCKz (eval_var _X (tarray tuint 16) rho)
   |-- !!(offset_val (Int.repr (sizeof tuint * ?25980))
            (eval_var _X (tarray tuint 16) rho) =
          force_val
            (sem_add_pi tuint (eval_var _X (tarray tuint 16) rho)
               (eval_id _i rho)) /\
          0 <= ?25980 /\
          ?25980 < LBLOCKz /\
          ?25981 = force_val (sem_cast_neutral (eval_id _l rho)))

subgoal 2 (ID 26561) is:
 semax Delta
   (PROP  ()
    LOCAL 
    (`(eq (offset_val (Int.repr (Z.of_nat i * 4 + 4)) data)) (eval_id _data);
    `(eq
        (Vint
           (big_endian_integer
              (fun z : Z =>
               force_int
                 (tuchars (map Int.repr (intlist_to_Zlist b))
                    (z + Z.of_nat i * 4)))))) (eval_id _l);
    `(eq
        (Vint
           (big_endian_integer
              (fun z : Z =>
               force_int
                 (tuchars (map Int.repr (intlist_to_Zlist b))
                    (z + Z.of_nat i * 4)))))) (eval_id _l');
    `(eq ctx) (eval_id _ctx);
    `(eq (Vint (Int.repr (Z.of_nat i)))) (eval_id _i);
    `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 0))) (eval_id _a);
    `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 1))) (eval_id _b);
    `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 2))) (eval_id _c);
    `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 3))) (eval_id _d);
    `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 4))) (eval_id _e);
    `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 5))) (eval_id _f);
    `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 6))) (eval_id _g);
    `(eq (Vint (nthi (Round regs (nthi b) (Z.of_nat i - 1)) 7))) (eval_id _h))
    
    SEP  (`(data_block sh (intlist_to_Zlist b) data);
    `K_vector (eval_var _K256 (tarray tuint 64));
    `(array_at tuint Tsh (upd (f_upto (tuints b) (Z.of_nat i)) ?25980 ?25981)
        0 LBLOCKz) (eval_var _X (tarray tuint 16))))
   (Ssequence
      (Sset _Ki
         (Ederef
            (Ebinop Oadd (Evar _K256 (tarray tuint 64)) 
               (Etempvar _i tint) (tptr tuint)) tuint)) MORE_COMMANDS)
   POSTCONDITION
*)


clear POSTCONDITION MORE_COMMANDS.
instantiate (2:= Z.of_nat i).
instantiate (1:= Vint (big_endian_integer
          (fun z : Z =>
           force_int
             (tuchars (map Int.repr (intlist_to_Zlist b))
                (z + Z.of_nat i * 4))))).
 apply prop_right; repeat split.

Focus 1. {
  rewrite <- H4.
  rewrite sem_add_pi_ptr by assumption.
  simpl force_val.
  unfold Int.mul.
  simpl sizeof.
  rewrite (Int.unsigned_repr 4) by (cbv; auto).
  rewrite (Int.unsigned_repr (Z.of_nat i)) by (assert (16 < Int.max_unsigned) by (cbv; auto); omega).
  reflexivity.
} Unfocus.

omega. 

change LBLOCKz with (Z.of_nat 16); apply Nat2Z.inj_lt; apply H0.

Focus 1. {
  rewrite <- H2.
  unfold sem_cast_neutral, force_val.
  reflexivity.
} Unfocus.

  assert_LOCAL (`(eq (Vint (nthi b (Z.of_nat i)))) (eval_id _l)).
  drop_LOCAL 6%nat. drop_LOCAL 5%nat. drop_LOCAL 4%nat.
  entailer. apply prop_right.
  unfold nthi.
  apply nth_big_endian_integer.
  rewrite Nat2Z.id. 
apply nth_error_nth; rewrite H; auto.
drop_LOCAL 2%nat; drop_LOCAL 2%nat.

change LBLOCKz with (Z.of_nat LBLOCK); rewrite loop1_aux_lemma1; auto.
(* 1,506,948 1,110,852 *)
(* 1,506,948 1,134,576 *)
assert (is_int (tuints K256 (Z.of_nat i))) 
 by abstract (clear - H0; apply ZnthV_map_Vint_is_int;
        split; try omega; apply Z.lt_trans with 16%Z; [omega | compute; auto]).
unfold K_vector.
forward.  (* Ki=K256[i]; *)
(* 1,689,280 1,212,872 *)

instantiate (1:= Zlength K256).
instantiate (1:= tuints K256).
instantiate (1:= Tsh).
abstract (
  assert (Zlength K256 = 64%Z) by reflexivity;
  entailer!; omega).
(* 1,811,028 1,406,332 *)
unfold POSTCONDITION, abbreviate; clear POSTCONDITION.

match goal with 
  |- semax _ (PROPx _ (LOCALx _ (SEPx ?R))) _ 
       (normal_ret_assert (PROPx ?P (LOCALx ?Q _)))
 => apply semax_post' with (PROPx P (LOCALx Q (SEPx R)));
  [ | change R with (nil++R); apply semax_frame_PQR with (R2:=R)]
end.
abstract (
apply andp_derives; auto;
apply andp_derives; auto;
unfold Z.succ; rewrite inj_S;
go_lower0; cancel).
auto 50 with closed.
(* 1,811,028 1,429,048 *)
forget (nthi b) as M.
apply semax_pre with
 (PROP  ()
   LOCAL 
   (`(eq ctx) (eval_id _ctx);
   `(eq (offset_val (Int.repr (Zsucc (Z.of_nat i) * 4)) data)) (eval_id _data);
   `(eq (Vint (W M (Z.of_nat i))))  (eval_id _l);
   `(eq (Vint (nthi K256 (Z.of_nat i)))) (eval_id _Ki);
   `(eq (Vint (Int.repr (Z.of_nat i)))) (eval_id _i);
   `(eq (Vint (nthi (Round regs M (Z.of_nat i - 1)) 0))) (eval_id _a);
   `(eq (Vint (nthi (Round regs M (Z.of_nat i - 1)) 1))) (eval_id _b);
   `(eq (Vint (nthi (Round regs M (Z.of_nat i - 1)) 2))) (eval_id _c);
   `(eq (Vint (nthi (Round regs M (Z.of_nat i - 1)) 3))) (eval_id _d);
   `(eq (Vint (nthi (Round regs M (Z.of_nat i - 1)) 4))) (eval_id _e);
   `(eq (Vint (nthi (Round regs M (Z.of_nat i - 1)) 5))) (eval_id _f);
   `(eq (Vint (nthi (Round regs M (Z.of_nat i - 1)) 6))) (eval_id _g);
   `(eq (Vint (nthi (Round regs M (Z.of_nat i - 1)) 7))) (eval_id _h))
   SEP()).
{ 
entailer!.
f_equal. f_equal. unfold Z.succ; rewrite Z.mul_add_distr_r; reflexivity.
rewrite W_equation.
rewrite if_true; auto.
clear - H0; change 16 with (Z.of_nat 16); apply Nat2Z.inj_lt; auto.
clear - H0 H2.
unfold tuints, ZnthV in H2.
rewrite Int.signed_repr in H2 by repable_signed.
rewrite if_false in H2 by omega.
unfold nthi.
rewrite Nat2Z.id in *.
rewrite (nth_map' _ _ Int.zero) in H2.
congruence.
clear H2.
change (length K256) with 64%nat.
simpl; omega.
}
{clear b H.
replace (Z.succ (Z.of_nat i) - 1)%Z with (Z.of_nat i) by omega.
rewrite (Round_equation _ _ (Z.of_nat i)).
rewrite if_false by omega.
forget (nthi K256 (Z.of_nat i)) as k.
forget (W M (Z.of_nat i)) as w.
assert (length (Round regs M (Z.of_nat i - 1)) = 8)%nat.
apply length_Round; auto.
forget (Round regs M (Z.of_nat i - 1)) as regs'.
change 16 with LBLOCK.
destruct regs' as [ | a [ | b [ | c [ | d [ | e [ | f [ | g [ | h [ | ]]]]]]]]]; inv H.
unfold rearrange_regs.
abbreviate_semax.
forward. (* T1 = l + h + Sigma1(e) + Ch(e,f,g) + Ki; *)
rewrite <- Sigma_1_eq, <- Ch_eq, rearrange_aux.
forward. (* T2 = Sigma0(a) + Maj(a,b,c); *)
 rewrite <- Sigma_0_eq, <- Maj_eq.
forward.
forward.
forward.
forward.
forward.
forward.
forward.
forward.
simplify_Delta.
 entailer!. 
unfold rnd_function, nthi; simpl.
f_equal.
 rewrite <- rearrange_aux. symmetry. rewrite (rearrange_aux Ki).
rewrite Int.add_commut.
repeat rewrite Int.add_assoc. reflexivity.
unfold rnd_function, nthi; simpl.
f_equal.
symmetry. do 2 rewrite Int.add_assoc.
rewrite Int.add_commut. rewrite <- Int.add_assoc. f_equal.
f_equal. rewrite Int.add_assoc. reflexivity.
}
}
Qed.
